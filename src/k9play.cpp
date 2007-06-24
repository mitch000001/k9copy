//
// C++ Implementation: k9play
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9play.h"
#include "k9dvdread.h"
#include "k9cell.h"
#include "k9vamps.h"

#include "dvdnav.h"
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dvdread.h"
#include <ktempfile.h>
#include <kstandarddirs.h>

void k9play::saveStatus(k9play_st _status) {
   QFile fstatus(m_inject);
   fstatus.open(IO_WriteOnly);
   fstatus.writeBlock((const char*)&_status,sizeof(k9play_st));
   fstatus.close();
   kdebug (QString("saving status : %1 %2 %3 %4 %5 %6  %7\n").arg(_status.title).arg(_status.chapter).arg(_status.cell).arg(_status.sector).arg(_status.bytesWritten).arg(_status.bytesRead).arg(_status.bytesSkipped));

} 

void k9play::readStatus(k9play_st &_status) {
   QFile fstatus(m_inject);
   if (fstatus.open(IO_ReadOnly)) {
	fstatus.readBlock((char*)&_status,sizeof(k9play_st));
	fstatus.close();
   } else memset(&_status,0,sizeof(k9play_st));

   kdebug (QString("reading status : title:%1 chapter:%2 cell:%3 sector:%4 written:%5 readen:%6 skipped:%7 chapters:%8  \n").arg(_status.title).arg(_status.chapter).arg(_status.cell).arg(_status.sector).arg(_status.bytesWritten).arg(_status.bytesRead).arg(_status.bytesSkipped).arg(_status.bytesChapters));

}


k9play::k9play() {
    m_stderr.open(IO_WriteOnly,stderr);
    m_startSector=0xFFFFFFFF;
    m_endSector=0xFFFFFFFF;
    m_vampsFactor=1;
    m_inputSize=1;
    m_chapterSize=0;
    m_chapter=0;
    m_cell=0;
    m_totalSize=0;
    m_forcedFactor=false;
    m_firstPass=false;
}

void k9play::kdebug(QString const & _msg) {
    #ifdef debug
    m_stderr.writeBlock(_msg.latin1(),_msg.length());
    #endif
}

void k9play::writeOutput(QString const & _msg) {
    m_stderr.writeBlock(_msg.latin1(),_msg.length());
}


k9play::~k9play() {
    m_stderr.close();
}

void k9play::setstartSector(QString _value) {
    if (_value !="")
    	m_startSector=_value.toUInt();
}

void k9play::setinject(QString _value) {
    m_inject=_value;
}

void k9play::setendSector(QString _value) {
    if (_value!="")
    	m_endSector=_value.toUInt();
}

void k9play::setaudioFilter( QString _value) {
    if (_value!="")
    	m_audioFilter=QStringList::split(",",_value);
}

void k9play::setsubpictureFilter( QString _value) {
    if (_value!="")
        m_subpictureFilter=QStringList::split(",",_value);
}

void k9play::setchapterList( QString _value) {
    if (_value!="")
        m_chapterList=QStringList::split(",",_value);
}

void k9play::setvampsFactor(QString _value) {
    if (_value!="")
    	m_vampsFactor=_value.toDouble();
}

void k9play::setinputSize( QString _value) {
    if (_value!="")
    	m_inputSize=_value.toULongLong();
}

void k9play::settotalSize( QString _value) {
    if (_value!="")
    	m_totalSize=_value.toULongLong();
}

void k9play::setdvdSize( QString _value) {
    if (_value!="")
    	m_dvdSize=_value.toULongLong();
}

void k9play::setchapterSize( QString _value) {
    if (_value!="")
    	m_chapterSize=_value.toULongLong();
}


void k9play::setchapter( QString _value) {
   if (_value!="")
	m_chapter=_value.toUInt();
}

void k9play::setcell(QString _value) {
   if (_value !="")
	m_cell=_value.toUInt();
}

void k9play::setinitStatus(bool _value) {
   m_initstatus=_value;
}

void k9play::setcontinue(bool _value) {
   m_continue=_value;
}

void k9play::setfirstPass(bool _value) {
   m_firstPass=_value;
}

void k9play::setforcedFactor( bool _value) {
   m_forcedFactor=_value;	
}

void k9play::execute() {
    //playCell();
    play();
    return;
}


void k9play::insert_nav_pack (int8_t *buf)
{
  int8_t *ptr = (int8_t*)buf;
  static uint8_t nav_pack1 [] =
  {
    /* pack header: SCR=0, mux rate=10080000bps, stuffing length=0 */
    0, 0, 1, 0xba, 0x44, 0x00, 0x04, 0x00, 0x04, 0x01, 0x01, 0x89, 0xc3, 0xf8,
    /* system header */
    0, 0, 1, 0xbb, 0x00, 0x12,
    /* contents of system header filled in at run time (18 bytes) */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    /* PES header for first private stream 2 packet */
    0, 0, 1, 0xbf, 0x03, 0xd4
  };
  static uint8_t nav_pack2 [] =
  {
    /* PES header for second private stream 2 packet */
    0, 0, 1, 0xbf, 0x03, 0xfa
  };

  memcpy (ptr, nav_pack1, sizeof (nav_pack1));
  ptr += sizeof (nav_pack1);
  memset (ptr, 0, DVD_VIDEO_LB_LEN/2 - sizeof (nav_pack1));
  ptr = buf + DVD_VIDEO_LB_LEN/2;
  memcpy (ptr, nav_pack2, sizeof (nav_pack2));
  ptr += sizeof (nav_pack2);
  memset (ptr, 0, DVD_VIDEO_LB_LEN/2 - sizeof (nav_pack2));
}

void k9play::insert_dummy_pack (int8_t *buf)
{
  int8_t *ptr = buf;
  static uint8_t dummy_pack [] =
  {
    /* pack header: SCR=0, mux rate=10080000bps, stuffing length=0 */
    0, 0, 1, 0xba, 0x44, 0x00, 0x04, 0x00, 0x04, 0x01, 0x01, 0x89, 0xc3, 0xf8,
    /* PES header for dummy video packet */
    0, 0, 1, 0xe0, 0x07, 0xec, 0x81, 0x00, 0x00
  };

  memcpy (ptr, dummy_pack, sizeof (dummy_pack));
  ptr += sizeof (dummy_pack);
  memset (ptr, 0xff, DVD_VIDEO_LB_LEN - sizeof (dummy_pack));
}



/* which is the default language for menus/audio/subpictures? */
#define DVD_LANGUAGE "en"
#define DVD_READ_CACHE 1

void k9play::play() {
    dvdnav_t *dvdnav;
    uint8_t mem[DVD_VIDEO_LB_LEN];
    int finished = 0;
    bool skipped=false;
    int32_t tt = 0,ptt=0;
    uint32_t pos, lgr;
    uint currCell=0;
    m_pos=0xFFFFFFFF;
    k9play_st status;

    if (m_initstatus)
	memset(&status,0,sizeof(k9play_st));
    else {
	readStatus( status);
	if (m_continue) 
	    m_startSector=status.sector;
    }

    KTempFile *bufferFile=new KTempFile(locateLocal("tmp", "k9copy/k9p"), "");
    m_output=bufferFile->file();

   // m_output.open(IO_WriteOnly,stdout);
    k9vamps vamps(NULL);
    vamps.reset();
    vamps.setPreserve( false);
    vamps.setOutput(m_output);

    // if reading of previous cell reached end of chapter, don't seek for cell
    if (m_chapter !=0 && m_cell !=0) {
	if (m_cell==1) {
	    if (status.bytesRead <=status.bytesChapters)
	    	status.bytesSkipped = status.bytesChapters - status.bytesRead;
	    else
		status.bytesSkipped=0;
	    status.bytesChapters += m_chapterSize;
        }
	if (status.title == m_title &&
	    status.chapter > m_chapter) {
		skipped=true;
	    }
    }

    //vamps.setVapFactor( m_vampsFactor);
    if (m_totalSize>0 && !m_forcedFactor) {
	double factor;
	factor = (double) (m_totalSize - (status.bytesRead +status.bytesSkipped)) / (double) (m_dvdSize-status.bytesWritten) ;
	if (factor <1) factor =1;
	kdebug(QString("shrink factor %1 totalSize:%2 (status.bytesRead +status.bytesSkipped):%3 m_dvdSize:%4 status.bytesWritten:%5").arg(factor).arg(m_totalSize).arg(status.bytesRead +status.bytesSkipped).arg(m_dvdSize).arg(status.bytesWritten) );
	vamps.setVapFactor(factor);
    } else {
	vamps.setVapFactor(m_vampsFactor);
	kdebug(QString("vamps factor %1\n").arg(m_vampsFactor));
    }


    vamps.setInputSize(m_inputSize);
    for ( QStringList::Iterator it = m_audioFilter.begin(); it != m_audioFilter.end(); ++it ) {
        vamps.addAudio((*it).toInt());
    }

    for ( QStringList::Iterator it = m_subpictureFilter.begin(); it != m_subpictureFilter.end(); ++it ) {
        vamps.addSubpicture((*it).toInt());
    }

    /* open dvdnav handle */
    if (dvdnav_open(&dvdnav, m_device,NULL) != DVDNAV_STATUS_OK) {
        writeOutput("ERR:Error on dvdnav_open\n");
        return ;
    }

    /* set read ahead cache usage */
    if (dvdnav_set_readahead_flag(dvdnav, DVD_READ_CACHE) != DVDNAV_STATUS_OK) {
        writeOutput( QString("ERR:Error on dvdnav_set_readahead_flag: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return;
    }

    /* set the language */
    if (dvdnav_menu_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK ||
            dvdnav_audio_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK ||
            dvdnav_spu_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK) {
        writeOutput( QString("ERR:Error on setting languages: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return ;
    }

    /* set the PGC positioning flag to have position information relatively to the
     * whole feature instead of just relatively to the current chapter */
    if (dvdnav_set_PGC_positioning_flag(dvdnav, 1) != DVDNAV_STATUS_OK) {
        writeOutput(QString("ERR:Error on dvdnav_set_PGC_positioning_flag: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return ;
    }

    int32_t parts;
    dvdnav_get_number_of_parts(dvdnav , m_title, &parts);

    if (m_chapter ==0) 
    	dvdnav_title_play(dvdnav , m_title);
    else {
	dvdnav_part_play(dvdnav, m_title, m_chapter);
	ptt=m_chapter;
    }
    /* the read loop which regularly calls dvdnav_get_next_block
     * and handles the returned events */
    bool bcopy=false;
    bool bcell=true;


    while (!finished && !skipped) {
        int result, event, len;
        uint8_t *buf = mem;

        /* the main reading function */
#if DVD_READ_CACHE

        result = dvdnav_get_next_cache_block(dvdnav, &buf, &event, &len);
#else

        result = dvdnav_get_next_block(dvdnav, buf, &event, &len);
#endif


        if (result == DVDNAV_STATUS_ERR) {
            writeOutput(QString("ERR:Error getting next block: %1\n").arg(dvdnav_err_to_string(dvdnav)));
            return;
        }
        switch (event) {
        case DVDNAV_NAV_PACKET:
            {
		dvdnav_current_title_info(dvdnav, &tt, &ptt);
		dvdnav_get_position(dvdnav, &pos, &lgr);
                m_length=lgr;
		status.title=tt;
		status.chapter=ptt;
		status.cell=currCell;
		status.sector=pos;

		if ((m_endSector !=0xFFFFFFFF) && (((status.bytesRead+status.bytesSkipped)/2048) >m_endSector)) {
			finished=1;
			kdebug(QString("pos >m_endSector %1 %2").arg((status.bytesRead+status.bytesSkipped)/2048).arg(m_endSector));
		}
		if ((m_chapter !=0 && ptt !=m_chapter) || (tt != m_title))
			finished=1;
 	    	if (m_cell!=0 && currCell>m_cell)
			finished=1;

		if (!finished && m_chapterList.count() >0) {
		    if (m_chapterList.findIndex( QString::number(ptt)) == -1) {
		        dvdnav_part_play(dvdnav,tt, ptt+1);
		        kdebug( QString("skipping chapter %1").arg(ptt));
		        continue;
		    	//dvdnav_part_play(dvdnav_t *self, int32_t title, int32_t part);
		    }
		    
		}
	
		if (m_continue) {
			dvdnav_sector_search(dvdnav,m_startSector , SEEK_SET);
			kdebug (QString("repositionning on %1").arg(m_startSector));
			m_continue=false;
			finished=0;
			bcell=true;
		} else {
			if ((m_cell==0  || (m_cell!=0 && currCell==m_cell)) && finished==0) {
    			    if (!vamps.running())
				vamps.start(QThread::NormalPriority);
			    bcopy=true;
			    vamps.addData( buf,len);
			    status.bytesRead +=len;
			    //writeOutput(QString("\rINFOPOS: %1 %2").arg((status.bytesRead+status.bytesSkipped) / DVD_VIDEO_LB_LEN).arg(lgr));
                            if (m_pos==0xFFFFFFFF)
                                m_pos=(status.bytesRead+status.bytesSkipped) / DVD_VIDEO_LB_LEN;
			}

		}

            }
	    break;
	//removed break --> save
        case DVDNAV_BLOCK_OK:
            /* We have received a regular block of the currently playing MPEG stream.*/
 	    if (m_cell==0  || (m_cell!=0 && currCell==m_cell)) {
 	        if (!vamps.running())
		    vamps.start(QThread::NormalPriority);
		vamps.addData( buf,len);
		status.bytesRead +=len;
                bcopy=true;
	    }
            break;
        case DVDNAV_NOP:
            /* Nothing to do here. */
            break;
        case DVDNAV_STILL_FRAME:
            /* We have reached a still frame. A real player application would wait
             * the amount of time specified by the still's length while still handling
             * user input to make menus and other interactive stills work.
             * A length of 0xff means an indefinite still which has to be skipped
             * indirectly by some user interaction. */
            {
                dvdnav_still_skip(dvdnav);
            }
            break;
        case DVDNAV_WAIT:
            /* We have reached a point in DVD playback, where timing is critical.
             * Player application with internal fifos can introduce state
             * inconsistencies, because libdvdnav is always the fifo's length
             * ahead in the stream compared to what the application sees.
             * Such applications should wait until their fifos are empty
             * when they receive this type of event. */
            dvdnav_wait_skip(dvdnav);
            break;
        case DVDNAV_SPU_CLUT_CHANGE:
            /* Player applications should pass the new colour lookup table to their
             * SPU decoder */
            break;
        case DVDNAV_SPU_STREAM_CHANGE:
            /* Player applications should inform their SPU decoder to switch channels */
            break;
        case DVDNAV_AUDIO_STREAM_CHANGE:
            /* Player applications should inform their audio decoder to switch channels */
            break;
        case DVDNAV_HIGHLIGHT:
            /* Player applications should inform their overlay engine to highlight the
             * given button */
            break;
        case DVDNAV_VTS_CHANGE:
            /* Some status information like video aspect and video scale permissions do
             * not change inside a VTS. Therefore this event can be used to query such
             * information only when necessary and update the decoding/displaying
             * accordingly. */
            break;
        case DVDNAV_CELL_CHANGE:
	    if (bcell) {
		currCell++;
		dvdnav_get_position(dvdnav, &pos, &lgr);
		status.title=tt;
		status.chapter=ptt;
		status.cell=currCell;
		status.sector=pos;

                flush();
                delete bufferFile;
                bufferFile=new KTempFile(locateLocal("tmp", "k9copy/k9p"), "");
                m_output=bufferFile->file();
                vamps.setOutput(m_output);
	    }
            break;
        case DVDNAV_HOP_CHANNEL:
            /* This event is issued whenever a non-seamless operation has been executed.
             * Applications with fifos should drop the fifos content to speed up responsiveness. */
            break;
        case DVDNAV_STOP:
            /* Playback should end here. */
            {
                finished = 1;
            }
            break;
        default:
            finished = 1;
            break;
        }

#if DVD_READ_CACHE
        dvdnav_free_cache_block(dvdnav, buf);
#endif

    }
    vamps.setNoData();
    vamps.wait();
    /* destroy dvdnav handle */
    dvdnav_close(dvdnav);

    if (! bcopy) {
	int8_t buf[DVD_VIDEO_LB_LEN];
	insert_nav_pack(buf);
	m_output->writeBlock((const char*)buf,DVD_VIDEO_LB_LEN);
	insert_dummy_pack(buf);
	m_output->writeBlock((const char*)buf,DVD_VIDEO_LB_LEN);

    }
    flush();    
    status.bytesWritten +=vamps.getOutputBytes();
    if (!m_firstPass)
       saveStatus( status);
    delete bufferFile;
}

void k9play::flush() {
    char buffer[DVD_VIDEO_LB_LEN];
    m_output->reset();
    QFile out;
    out.open(IO_WriteOnly,stdout);
    while(!m_output->atEnd()) {
        writeOutput(QString("\rINFOPOS: %1 %2").arg(m_pos).arg(m_length));
        m_pos++;
        int l=m_output->readBlock(buffer,DVD_VIDEO_LB_LEN);
        if (l>0)
            out.writeBlock(buffer,DVD_VIDEO_LB_LEN);
    }
    m_output->close();
    m_output->remove();
    m_pos=0xFFFFFFFF;
}

bool k9play::readNavPack (k9DVDFile *fh, dsi_t *dsi,int sector,uchar *_buffer)
{
  int n;
  /* try max_read_retries+1 times */
    n = fh->readBlocks( sector, 1,_buffer);

    if (n == 1)
    {
      /* read Ok */
      if (k9Cell::isNavPack (_buffer))
         /* parse contained DSI pack */
          DvdreadF()->navRead_DSI (dsi, _buffer + DSI_START_BYTE);
          if (sector == dsi -> dsi_gi.nv_pck_lbn) {
               return true;
          }
    }
    return false;
}





