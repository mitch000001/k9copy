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

k9play::k9play() {
    m_stderr.open(IO_WriteOnly,stderr);
}


k9play::~k9play() {
    m_stderr.close();
}

void k9play::setstartSector(QString _value) {
    QString dbg="start sector :" + _value;
    m_stderr.writeBlock(dbg,dbg.length());	
    m_startSector=_value.toUInt();
}

void k9play::setendSector(QString _value) {
    m_endSector=_value.toUInt();
}

uint32_t k9play::copyVobu(k9DVDFile  *_fileHandle,uint32_t _startSector) {
    dsi_t	dsi_pack;

    uint32_t	nsectors, len=0;
    uchar *buf;
    uint32_t sector=_startSector;
    /* read nav pack */
    buf=(uchar*) malloc(DVD_VIDEO_LB_LEN);
    _fileHandle->readBlocks ( sector, 1, buf);

    m_output.writeBlock((char*)buf,DVD_VIDEO_LB_LEN);

    /* parse contained DSI pack */
    navRead_DSI (&dsi_pack, buf + DSI_START_BYTE);

    nsectors      = dsi_pack.dsi_gi.vobu_ea;
    uint32_t dsi_next_vobu = dsi_pack.vobu_sri.next_vobu;

    if ((sector >= m_startSector)    && (sector <=m_endSector)) {
	
	for (int i=0; i <nsectors;i++) {
		/* read VOBU */
		len += _fileHandle->readBlocks ( (sector + 1 +i), 1, buf);
		m_output.writeBlock((char*)buf,DVD_VIDEO_LB_LEN);
	}
	m_totalBytes+=(len+1);
    }

    free(buf);

    return dsi_next_vobu;
}



void k9play::execute() {
    m_totalBytes=0;
    m_dvd=new k9DVD();
    m_dvd->scandvd(m_device,true);
    k9DVDTitle *title=NULL;
    bool foundtitle=false;
    for (uint i=0;i<m_dvd->gettitleCount();i++) {
        title=m_dvd->gettitle(i);
        if (title->getnumTitle() == m_title) {
            foundtitle=true;
            //	 qDebug("found title" +QString::number(m_title));
            break;
        }
    }

    if (foundtitle) {
	m_dvdTitle=title;
        k9DVDRead dvdread;
        dvdread.openDevice(m_device);
        m_output.open( IO_WriteOnly, stdout );
        QFile input;
        input.open( IO_ReadOnly,stdin);

        k9DVDFile *dvdfile=dvdread.openTitle(title->getVTS());

        k9DVDChapter *chapter;
        bool stop=false;

	m_stderr.writeBlock(QString::number(m_startSector),QString::number(m_startSector).length());
	m_stderr.writeBlock(QString::number(m_endSector),QString::number(m_endSector).length());
        for (uint i=0; i<title->getchapterCount();i++) {
            chapter=title->getChapter(i);
            for (uint iCell=0;iCell <chapter->cells.count();iCell++) {
                k9ChapterCell *cell=chapter->cells.at(iCell);
		if ((cell->getangle() ==1) && (cell->getstartSector()<=m_endSector) && (cell->getlastSector()>=m_startSector)) {
			uint32_t dsi_next_vobu=0;
			for (uint32_t sector =   cell->getstartSector();
				dsi_next_vobu != SRI_END_OF_CELL; sector += dsi_next_vobu & 0x7fffffff) {
			dsi_next_vobu= copyVobu(dvdfile,sector);
			printPosition();
			}
		}
            }
        }
        m_output.close();
        input.close();
        dvdfile->close();
        dvdread.close();
    }


    m_dvd->close();
    delete m_dvd;


}

void k9play::printPosition() {
        double percent=m_totalBytes / (m_dvdTitle->getsize_mb()*512);
	
	QString spercent=QString("INFOPOS: %1 %2").arg(m_totalBytes+m_startSector).arg((uint32_t)(m_dvdTitle->getsize_mb()*512));
	m_stderr.writeBlock(spercent.latin1(),spercent.length());

}


