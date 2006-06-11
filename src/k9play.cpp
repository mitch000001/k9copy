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
#include "k9ifo.h"
#include "k9cell.h"
#include "k9vamps.h"

#include "dvdnav.h"
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


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
	k9play_st status;
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
}

void k9play::kdebug(QString const & _msg) {
    m_stderr.writeBlock(_msg.latin1(),_msg.length());
}

k9play::~k9play() {
    m_stderr.close();
}

void k9play::setstartSector(QString _value) {
    QString dbg="start sector :" + _value;
    kdebug(dbg);
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

void k9play::execute() {
    //playCell();
    play();
    return;
}


void k9play::printPosition() {
    QString spercent=QString("INFOPOS: %1 %2 \n").arg(m_totalBytes+m_startSector).arg((uint32_t)(m_dvdTitle->getsize_mb()*512));
    m_stderr.writeBlock(spercent.latin1(),spercent.length());

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
    int dump = 1, tt_dump = 0;
    bool skipped=false;
    int32_t tt = 0,ptt=0;
    uint32_t pos, lgr;
    uint currCell=0;

    k9play_st status;

    if (m_initstatus)
	memset(&status,0,sizeof(k9play_st));
    else
	readStatus( status);

    m_output.open(IO_WriteOnly,stdout);
    k9vamps vamps(NULL);
    vamps.reset();
    vamps.setPreserve( false);
    vamps.setOutput(&m_output);

    // if reading of previous cell reached end of chapter, don't seek for cell
    if (m_chapter !=0 && m_cell !=0) {
	if (m_cell==1) {
	    status.bytesSkipped = status.bytesChapters - status.bytesRead;
	    kdebug(QString("maj chaptersize : %1").arg(m_chapterSize));
	    status.bytesChapters += m_chapterSize;
        }
	if (status.title == m_title &&
	    status.chapter > m_chapter) {
		skipped=true;
	    }
    }

    //vamps.setVapFactor( m_vampsFactor);
    if (m_totalSize>0) {
	double factor;
//	factor = (double) (m_totalSize - status.bytesRead) / (double) (m_dvdSize-status.bytesWritten) ;

	factor = (double) (m_totalSize - (status.bytesRead +status.bytesSkipped)) / (double) (m_dvdSize-status.bytesWritten) ;
	if (factor <1) factor =1;
	kdebug( QString("computed factor %2 / %3 :%1").arg(factor).arg((double) (m_totalSize - (status.bytesRead + status.bytesSkipped)),0,'f',0).arg((double) (m_dvdSize-status.bytesWritten),0,'f',0) );
	vamps.setVapFactor(factor);
    } else
	vamps.setVapFactor(m_vampsFactor);

    vamps.setInputSize(m_inputSize);
    for ( QStringList::Iterator it = m_audioFilter.begin(); it != m_audioFilter.end(); ++it ) {
        vamps.addAudio((*it).toInt());
    }

    for ( QStringList::Iterator it = m_subpictureFilter.begin(); it != m_subpictureFilter.end(); ++it ) {
        vamps.addSubpicture((*it).toInt());
    }

    /* open dvdnav handle */
    if (dvdnav_open(&dvdnav, m_device) != DVDNAV_STATUS_OK) {
        kdebug("ERR:Error on dvdnav_open\n");
        return ;
    }

    /* set read ahead cache usage */
    if (dvdnav_set_readahead_flag(dvdnav, DVD_READ_CACHE) != DVDNAV_STATUS_OK) {
        kdebug(QString("ERR:Error on dvdnav_set_readahead_flag: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return;
    }

    /* set the language */
    if (dvdnav_menu_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK ||
            dvdnav_audio_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK ||
            dvdnav_spu_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK) {
        kdebug(QString("ERR:Error on setting languages: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return ;
    }

    /* set the PGC positioning flag to have position information relatively to the
     * whole feature instead of just relatively to the current chapter */
    if (dvdnav_set_PGC_positioning_flag(dvdnav, 1) != DVDNAV_STATUS_OK) {
        kdebug(QString("ERR:Error on dvdnav_set_PGC_positioning_flag: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return ;
    }

    int32_t parts;
    dvdnav_get_number_of_parts(dvdnav , m_title, &parts);
    kdebug(QString("number of parts %1 \n").arg(parts));

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
            kdebug(QString("ERR:Error getting next block: %1\n").arg(dvdnav_err_to_string(dvdnav)));
            return;
        }

        switch (event) {
        case DVDNAV_NAV_PACKET:
            {
		dvdnav_current_title_info(dvdnav, &tt, &ptt);
		dvdnav_get_position(dvdnav, &pos, &lgr);
		status.title=tt;
		status.chapter=ptt;
		status.cell=currCell;
		status.sector=pos;

		if ((m_endSector !=0xFFFFFFFF) && (pos >m_endSector))
			finished=1;
		if ((m_chapter !=0 && ptt !=m_chapter) || (tt != m_title))
			finished=1;
 	    	if (m_cell!=0 && currCell>m_cell)
			finished=1;

	
		if (m_startSector!=0xFFFFFFFF) {
			kdebug("\nRepositionning ....\n");
			uint32_t lg2;
			dvdnav_sector_search(dvdnav,m_startSector , SEEK_SET);

			m_startSector=0xFFFFFFFF;
			finished=0;
			bcell=true;
		} else {
			if ((m_cell==0  || (m_cell!=0 && currCell==m_cell)) && finished==0) {
    			    if (!vamps.running())
				vamps.start(QThread::NormalPriority);
			    bcopy=true;
			    vamps.addData( buf,len);
			    status.bytesRead +=len;
			    kdebug(QString("\rINFOPOS: %1 %2").arg((status.bytesRead+status.bytesSkipped) / DVD_VIDEO_LB_LEN).arg(lgr));
			}

		}


                pci_t *pci;
                dsi_t *dsi;

                /* Applications with fifos should not use these functions to retrieve NAV packets,
                 * they should implement their own NAV handling, because the packet you get from these
                 * functions will already be ahead in the stream which can cause state inconsistencies.
                 * Applications with fifos should therefore pass the NAV packet through the fifo
                 * and decoding pipeline just like any other data. */
                pci = dvdnav_get_current_nav_pci(dvdnav);
                dsi = dvdnav_get_current_nav_dsi(dvdnav);

                if(pci->hli.hl_gi.btn_ns > 0) {
                    int button;
                    button = 1;
                    dvdnav_button_select_and_activate(dvdnav, pci, button);
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
   	      //  dvdnav_get_position(dvdnav, &pos, &lgr);
	       // kdebug(QString("\rINFOPOS: %1 %2").arg(pos).arg(lgr));
	    }
//	    if (m_cell!=0 && currCell>m_cell)
//		finished=1;
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
                dvdnav_still_event_t *still_event = (dvdnav_still_event_t *)buf;
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
            {
                dvdnav_highlight_event_t *highlight_event = (dvdnav_highlight_event_t *)buf;
            }
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

		kdebug(QString("\nCell changed: %1  position:%2\n").arg(currCell).arg(pos));
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
//	kdebug(QString(dvdnav_err_to_string(dvdnav)));

#if DVD_READ_CACHE
        dvdnav_free_cache_block(dvdnav, buf);
#endif

    }
    vamps.setNoData();
    vamps.wait();
    /* destroy dvdnav handle */
    dvdnav_close(dvdnav);

    if (! bcopy) {
        kdebug ("\ninsert dummy pack\n");
	int8_t buf[DVD_VIDEO_LB_LEN];
	insert_nav_pack(buf);
	m_output.writeBlock((const char*)buf,DVD_VIDEO_LB_LEN);
	insert_dummy_pack(buf);
	m_output.writeBlock((const char*)buf,DVD_VIDEO_LB_LEN);

    }

    m_output.close();
    status.bytesWritten +=vamps.getOutputBytes();
    saveStatus( status);
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
          navRead_DSI (dsi, _buffer + DSI_START_BYTE);
          if (sector == dsi -> dsi_gi.nv_pck_lbn) {
               return true;
          }
    }
    return false;
}


void k9play::playCell() {
  k9DVDRead dvdreader;
  k9DVDFile *dvdfile;
  
  pgc_t *       pgc;
  int           sector, first_sector, next_vobu = 0;

  dvdreader.openDevice( m_device); 
  k9Ifo ifo(&dvdreader);
  
  //temporary
  int vts_num=1;
  int pgc_num=3;
  int cell=2;

    m_output.open(IO_WriteOnly,stdout);
    k9vamps vamps(NULL);
    vamps.reset();
    vamps.setOutput(&m_output);
    vamps.setVapFactor( m_vampsFactor);
    vamps.setInputSize(m_inputSize);
    for ( QStringList::Iterator it = m_audioFilter.begin(); it != m_audioFilter.end(); ++it ) {
        vamps.addAudio((*it).toInt());
    }

    for ( QStringList::Iterator it = m_subpictureFilter.begin(); it != m_subpictureFilter.end(); ++it ) {
        vamps.addSubpicture((*it).toInt());
    }

  ifo.setDevice(m_device);
  ifo.openIFO( vts_num);
  dvdfile = dvdreader.openTitle( vts_num);
   

  pgc = ifo.getIFO()-> vts_pgcit -> pgci_srp [pgc_num - 1].pgc;

  first_sector = pgc -> cell_playback [cell - 1].first_sector;

 // vamps.start(QThread::NormalPriority);

  /* loop until out of the cell */
  for (sector = first_sector;
       next_vobu != SRI_END_OF_CELL; sector += next_vobu & 0x7fffffff)
  {
    dsi_t dsi;
    int   nsectors, len;
    uchar *buf1=(uchar*)malloc (DVD_VIDEO_LB_LEN);
    /* read nav pack */
    if (! readNavPack( dvdfile, &dsi, sector, buf1) ) {
      kdebug("failed to read nav pack");
      free (buf1);
      //if reading of nav pack failed, whe should look for the next vobu in ifo file
      return;
    }


    nsectors  = dsi.dsi_gi.vobu_ea;
    next_vobu = dsi.vobu_sri.next_vobu;
    uchar *buf=(uchar*) malloc(nsectors*DVD_VIDEO_LB_LEN);
    len =  dvdfile->readBlocks(sector + 1, nsectors,buf );
    if (len !=-1) {
        m_output.writeBlock((const char*)buf1, DVD_VIDEO_LB_LEN);
        m_output.writeBlock((const char*)buf,DVD_VIDEO_LB_LEN * nsectors);
    }
    free (buf1);
    free (buf);
  }    
//  vamps.setNoData();
//  vamps.wait();

  m_output.close(); 
  ifo.closeIFO();
  dvdfile->close();
  dvdreader.close();

}



