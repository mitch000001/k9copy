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



k9play::k9play() {
    m_stderr.open(IO_WriteOnly,stderr);
    m_startSector=0xFFFFFFFF;
    m_endSector=0xFFFFFFFF;
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
    m_startSector=_value.toUInt();
}

void k9play::setendSector(QString _value) {
    m_endSector=_value.toUInt();
}

void k9play::setaudioFilter( QString _value) {
    m_audioFilter=QStringList::split(",",_value);
}

void k9play::setsubpictureFilter( QString _value) {
    m_subpictureFilter=QStringList::split(",",_value);
}

void k9play::execute() {

    play();
    return;


}


void k9play::printPosition() {
    QString spercent=QString("INFOPOS: %1 %2 \n").arg(m_totalBytes+m_startSector).arg((uint32_t)(m_dvdTitle->getsize_mb()*512));
    m_stderr.writeBlock(spercent.latin1(),spercent.length());

}


/* which is the default language for menus/audio/subpictures? */
#define DVD_LANGUAGE "en"
#define DVD_READ_CACHE 1

void k9play::play() {
    dvdnav_t *dvdnav;
    uint8_t mem[DVD_VIDEO_LB_LEN];
    int finished = 0;
    int dump = 1, tt_dump = 0;

    int32_t tt = 0,ptt=0;
    uint32_t pos, lgr;

    m_output.open(IO_WriteOnly,stdout);
    k9vamps vamps(NULL);
    vamps.reset();
    vamps.setOutput(&m_output);
    vamps.setVapFactor( 1.0);
    //vamps.setInputSize( 
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

    dvdnav_title_play(dvdnav , m_title);
    vamps.start(QThread::NormalPriority);
    /* the read loop which regularly calls dvdnav_get_next_block
     * and handles the returned events */
    while (!finished) {
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
        case DVDNAV_BLOCK_OK:
            /* We have received a regular block of the currently playing MPEG stream.*/

            dvdnav_current_title_info(dvdnav, &tt, &ptt);
            dvdnav_get_position(dvdnav, &pos, &lgr);
            kdebug(QString("INFOPOS: %1 %2").arg(pos).arg(lgr));

            if (m_startSector!=0xFFFFFFFF) {
                dvdnav_sector_search(dvdnav, m_startSector, SEEK_SET);
                m_startSector=0xFFFFFFFF;
            } else {
                if (dump || tt_dump)
                    //m_output.writeBlock((const char*)buf, len);
		    vamps.addData( buf,len);
            }
            if ((m_endSector !=0xFFFFFFFF) && (pos >m_endSector))
                finished=1;

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
            /* Some status information like the current Title and Part numbers do not
             * change inside a cell. Therefore this event can be used to query such
             * information only when necessary and update the decoding/displaying
             * accordingly. */
            {

                //	printf("Cell change: Title %d, Chapter %d\n", tt, ptt);
                //	printf("At position %.0f%% inside the feature\n", 100 * (double)pos / (double)len);
                /*
                              dump = 0;
                              if (tt_dump && tt != tt_dump)
                                  tt_dump = 0;

                              if (!dump && !tt_dump) {
                                  dump = 1;
                              }
                */
            }
            break;
        case DVDNAV_NAV_PACKET:
            /* A NAV packet provides PTS discontinuity information, angle linking information and
             * button definitions for DVD menus. Angles are handled completely inside libdvdnav.
             * For the menus to work, the NAV packet information has to be passed to the overlay
             * engine of the player so that it knows the dimensions of the button areas. */
            {
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
    m_output.close();

    return ;
}
