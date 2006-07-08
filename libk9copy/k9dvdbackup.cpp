/***************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   jm_petit@laposte.net                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#include "k9dvdbackup.h"
#include "k9backupdlg.h"
#include "k9dvd.h"
#include "bswap.h"
#include "k9ifo.h"
#include "k9vamps.h"
#include "ac.h"
#include <kmessagebox.h>
#include <kapplication.h>
#include <qapplication.h>
#include <qfileinfo.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <klocale.h>
#include <qdir.h>
#include <kstandarddirs.h>

#define BUF_SECS	1024


#include "k9dvdbackup.moc"

int k9CadtList::compareItems(QPtrCollection::Item item1,QPtrCollection::Item item2) {
    cell_adr_t * it1=(cell_adr_t*)item1;
    cell_adr_t * it2=(cell_adr_t*)item2;
    return it1->start_sector - it2->start_sector;
};

int k9TitleSetList::compareItems ( QPtrCollection::Item item1, QPtrCollection::Item item2 ) {
    k9TitleSet * it1,*it2;
    it1=(k9TitleSet *) item1;
    it2=(k9TitleSet *)item2;
    return it1->VTS - it2->VTS;
}


k9TitleSet::k9TitleSet(int _VTS):QObject(NULL,"") {
    startSector=0;
    lastSector=0;
    VTS=_VTS;
    cells.setAutoDelete(true);
    ifoTitle=NULL;
}

k9TitleSet::~k9TitleSet() {
    ifoTitle->closeIFO();
    delete ifoTitle;
}

k9Cell* k9TitleSet::addCell(int _vts,int _pgc, int _vob) {
    return cells.addCell(_vts,_pgc,_vob);

}

uint32_t k9TitleSet::getSize() {
    return (lastSector +1) ;
}


k9DVDBackup::k9DVDBackup(QObject* _dvd,const char* name,const QStringList& args)
        : QObject(NULL, "") {
    DVD = (k9DVD*)_dvd;
    currVTS=0;
    outputFile=NULL;
    currTS=NULL;
    errMsg="";
    error=false;
    backupDlg = new k9BackupDlg(qApp->mainWidget(),"",true);
    //  cells.setAutoDelete(true);
    vamps=new k9vamps(this);
}


k9DVDBackup::~k9DVDBackup() {
    if (outputFile !=NULL) {
        outputFile->close();
        delete outputFile;
    }
    delete backupDlg;
    delete vamps;
}

bool k9DVDBackup::geterror() {
    return error;
}

void k9DVDBackup::seterror(const QString &_msg) {
    error=true;
    errMsg=_msg;
}


QString k9DVDBackup::getErrMsg() {
    return(errMsg);
}

void k9DVDBackup::prepareVTS(int _VTS) {
    if (error)
        return;
    ifo_handle_t *hifo;

    cellSize=0;
    if (currTS==NULL) {
        currVTS=0;
        m_vmgSize=copyMenu2(0);
        calcFactor();
        if (outputFile != NULL) {
            outputFile->close();
            delete outputFile;
            outputFile=NULL;
        }
    }
    if (backupDlg->getAbort()) {
        seterror(tr2i18n("DVD backup cancelled"));
        return;
    }
    //creation of output file
    if (currVTS != _VTS) {
        if (outputFile != NULL) {
            outputFile->close();
            delete outputFile;
            outputFile=NULL;
            updateIfo();
            updateVob(&currTS->cells);
        }

        if (backupDlg->getAbort()) {
            seterror(tr2i18n("DVD backup cancelled"));
            return;
        }

        uint32_t startSector=0;

        if (currTS != NULL) {
            startSector = currTS->startSector + currTS->getSize();
        } else {
            k9Ifo kifo(&m_dvdread);
            kifo.openIFO( 0);
            hifo = kifo.getIFO();
            if (!hifo) {
                seterror( tr2i18n("unable to open VIDEO_TS.IFO"));
                return;
            }
            startSector= hifo->vmgi_mat->vmg_last_sector+1  ;
            kifo.closeIFO();
        }

        currTS = new k9TitleSet(_VTS);
        currVOB=0;
        currVTS=_VTS;

        currTS->menuSize=copyMenu2(_VTS);
        if (outputFile != NULL) {
            outputFile->close();
            delete outputFile;
        }

        QString filename;
        filename.sprintf("/VTS_%02d_1.VOB",_VTS);
        filename=output+filename;
        outputFile=new QFile(filename);
        if (! outputFile->open(IO_WriteOnly)) {
            seterror(tr2i18n("Unable to open file ") + filename);
            return;
        }

        currVOB=1;

        currTS->startSector=startSector;
        currTS->lastSector += currTS->menuSize ;
        titleSets.append(currTS);

        k9Ifo *kifo =new k9Ifo(&m_dvdread);
        kifo->openIFO( _VTS);
        currTS->ifoTitle=kifo;

        m_position=0;
        m_copyMenu=false;
        calcFactor();
    }

    currVTS=_VTS;

}


void k9DVDBackup::copyCell(int _VTS,k9Cell * _cell,bool _empty) {
    if (error)
        return;

    prepareVTS(_VTS);
    if (error)
        return;
    mutex.lock();
    k9Cell *cell= currTS->addCell(_VTS,0,0);
    //JMP cell->startSector=m_position;
    cell->startSector=cell->cellList->getPosition();
    currCell=cell;
    //start cell copy
    mutex.unlock();
    if (!_empty) {
        if (!vamps->running()) {
            vamps->reset();
            vamps->setInputSize(argSize);
            QValueList<int>::iterator it;
            for ( it = _cell->audio.begin(); it != _cell->audio.end(); ++it ) {
                vamps->addAudio(*it);
            }

            for ( it = _cell->subpicture.begin(); it != _cell->subpicture.end(); ++it ) {
                vamps->addSubpicture(*it);
            }

            vamps->setVapFactor(argFactor);
            vamps->start(QThread::NormalPriority);
        }
        playCell(_VTS,_cell);
        //JMP	vamps->wait();
    } else
        copyEmptyPgc(_VTS,_cell);
    qApp->processEvents();
}

void k9DVDBackup::copyEmptyPgc(int _vts,k9Cell *_cell) {
    if (error)
        return;

    ifo_handle_t *vts_handle=currTS->ifoTitle->getIFO();
    ;

    k9DVDFile *dvdfile;
    if ((dvdfile = m_dvdread.openTitle( _vts))== 0) {
        QString stmp;
        stmp=i18n("Unable to open titleset %1").arg(_vts);
        seterror(stmp);
        return ;
    }
    backupDlg->setTotalSteps(vts_handle->vtsi_mat->vts_last_sector-vts_handle->vtsi_mat->vtstt_vobs  -1);
    QString c;
    c=i18n("Extracting titleset %1").arg(_vts);
    backupDlg->setProgressLabel(c);
    backupDlg->show();


    uint32_t sector,dsi_next_vobu = 0,len=0;
    uchar buffer[DVD_VIDEO_LB_LEN];
    currCell->oldStartSector=_cell->startSector;
    sector = _cell->startSector;
    backupDlg->setProgress(sector);
    dsi_t	 dsi_pack;
    uint32_t	 nsectors;

    dvdfile->readBlocks (sector, 1, buffer);


    currCell->oldLastSector=_cell->lastSector;
    k9Vobu * vobu=currCell->addVobu(sector);
    vobu->empty=true;
    currCell->addNewVobus((char*)buffer,DVD_VIDEO_LB_LEN,currCell->cellList->getPosition(),currVOB,outputFile->at());
    outputFile->writeBlock((char*)buffer,DVD_VIDEO_LB_LEN);

    /* parse contained DSI pack */
    navRead_DSI (&dsi_pack, buffer + DSI_START_BYTE);
    currCell->vob = dsi_pack.dsi_gi.vobu_vob_idn;
    //vobu->size=1;
    /* JMP
        if (dsi_pack.dsi_gi.vobu_ea != 0) {
            nsectors      = 1; //dsi_pack.dsi_gi.vobu_ea;
            dsi_next_vobu = dsi_pack.vobu_sri.next_vobu;
     
            uchar *buf=(uchar*) malloc(nsectors*DVD_VIDEO_LB_LEN);
     
            len = dvdfile->readBlocks ( (uint32_t) (sector + 1), nsectors, buf) +len ;
            outputFile->writeBlock((char*)buf,nsectors * DVD_VIDEO_LB_LEN);
            free(buf);
        } else {
            nsectors = 0;
        }
    */
    nsectors=0;
    len=0;


    vobu->size +=nsectors;
    currCell->lastSector=currCell->startSector+ len;  //JMP *DVD_VIDEO_LB_LEN;
    // position+=DVD_VIDEO_LB_LEN + len*DVD_VIDEO_LB_LEN;
    //JMP m_position+=1 + len;
    currCell->cellList->setPosition(currCell->cellList->getPosition()+1+len);
    currTS->lastSector+=len+1;

    dvdfile->close();
    backupDlg->setProgressTotal(len+1);
    m_outbytes+=DVD_VIDEO_LB_LEN;
    m_inbytes+=DVD_VIDEO_LB_LEN;
}


void k9DVDBackup::getOutput(uchar * buffer, uint32_t buflen) {
    if (error)
        return;
    mutex.lock();
    backupDlg->playMovie(buffer,buflen);
    mutex.unlock();

    m_outbytes+=buflen;

    uchar *temp =buffer;
    QString sName;
    if ((buflen %2048) !=0)
        qDebug("getOutput, buffer :" +QString::number(buflen));
    uint end=0;

    for (uint itemp=0;itemp<buflen;itemp+=DVD_VIDEO_LB_LEN) {
        if (buflen-itemp <DVD_VIDEO_LB_LEN)
            break;


        mutex.lock();
        long fileSize=outputFile->size();
        end=itemp+DVD_VIDEO_LB_LEN;
        if (k9Cell::isNavPack(temp+itemp)) {

            k9Vobu * vobu = vobuQueue.dequeue();

            cellOut=vobu->parent;
            dsi_t dsiPack;
            navRead_DSI (&dsiPack, (uchar*)(temp+itemp) + DSI_START_BYTE);

            cellOut->vob = dsiPack.dsi_gi.vobu_vob_idn;
            if ((dsiPack.dsi_gi.vobu_ea * DVD_VIDEO_LB_LEN) + fileSize >= (1024*1024*1024)) {
                outputFile->close();
                delete outputFile;
                currVOB++;
                if (currVTS==0)
                    sName = "/VIDEO_TS.VOB";
                else
                    sName.sprintf("/VTS_%02d_%d.VOB",currVTS,currVOB);
                sName=output+sName;
                outputFile=new QFile(sName);
                if ( !outputFile->open(IO_WriteOnly)) {
                    seterror(tr2i18n("Unable to open file ") + sName);
                    mutex.unlock();
                    return;
                }
            }
        }
        cellOut->addNewVobus((char*)(temp+itemp),DVD_VIDEO_LB_LEN,cellOut->cellList->getPosition() ,currVOB,outputFile->at());
        outputFile->writeBlock((char*)(temp+itemp),DVD_VIDEO_LB_LEN);

        backupDlg->setProgressTotal(1);
        cellOut->cellList->setPosition( cellOut->cellList->getPosition()+1);
        if (!m_copyMenu)
            currTS->lastSector++;
        mutex.unlock();
    }
}


/*!
\fn k9DVDBackup::setDevice(QString _device)
*/
void k9DVDBackup::setDevice(QString _device) {
    device=_device;
}


/*!
\fn k9DVDBackup::setOutput(QString _output)
*/
void k9DVDBackup::setOutput(QString _output) {
    output=QDir::cleanDirPath(_output);
}


uint32_t k9DVDBackup::copyMenu2(int _vts) {
    if (error )
        return 0;
    k9Ifo kifo(&m_dvdread);
    kifo.openIFO( _vts);
    ifo_handle_t *hifo =kifo.getIFO();
    m_ifo=hifo;
    uint32_t msize=0;
    if (_vts==0)
        msize=hifo->vmgi_mat->vmg_last_sector -1  - 2* hifo->vmgi_mat->vmgi_last_sector;
    else
        msize=hifo->vtsi_mat->vtstt_vobs - hifo->vtsi_mat->vtsi_last_sector -1;

    if (msize==0) {
        kifo.closeIFO();
        return 0;
    }


    m_position=0;
    m_copyMenu=true;
    QString targetName;
    if (_vts == 0) {
        targetName="VIDEO_TS.VOB";
    } else {
        targetName.sprintf("VTS_%02i_0.VOB",_vts);
    }
    targetName=output+"/"+targetName;

    outputFile=new QFile(targetName);
    if (! outputFile->open(IO_WriteOnly)) {
        seterror(tr2i18n("Unable to open file ") + targetName);
        return 0;
    }

    k9DVDFile *dvdfile;
    if ((dvdfile = m_dvdread.openMenu( _vts))== 0) {
        QString stmp;
        stmp=i18n("Unable to open menu for titleset %1").arg(_vts);
        seterror (stmp);
        return 0;
    }

    k9CellList *lstCell;
    if (_vts==0)
        lstCell = &vmgCells;
    else
        lstCell = &currTS->menuCells;

    lstCell->setAutoDelete(true);


    c_adt_t *c_adt = hifo->menu_c_adt;
    // c_adt shouldn't be null. It would say that the menu doesn't contain video !?
    if (c_adt==NULL)
	return 0;

    uint32_t length = c_adt->last_byte + 1 - C_ADT_SIZE;
    cell_adr_t *ptr;
    ptr= c_adt->cell_adr_table;

    uint32_t sector, dsi_next_vobu = 0;
    uint32_t imax=length/sizeof(cell_adr_t);
    QString c;
    c=i18n("Extracting menu for titleset %1").arg(_vts);
    backupDlg->setProgressLabel(c);

    backupDlg->show();
    backupDlg->setTotalSteps(ptr[imax-1].last_sector);

    k9CadtList cadr;

    for (uint32_t i=0;i<imax;i++) {
        cadr.append(ptr+i);
    }
    cadr.sort();
    vamps->reset();
    for (uint i=1;i<=8;i++)
        vamps->addAudio(i);

    for (uint i=1;i<=32;i++)
        vamps->addSubpicture(i);

    vamps->setVapFactor(argFactor);
    vamps->setInputSize(msize*2048);

    vamps->start(QThread::NormalPriority);
    //	while(!vamps->running() && !vamps->finished());

    for(uint32_t i = 0; i < imax; i++) {
        currCell=lstCell->addCell(_vts,1,1);
        dsi_next_vobu=0;
        cell_adr_t * cellAdr=cadr.at(i);
        for (sector = cellAdr->start_sector;
                sector <= cellAdr->last_sector; sector += dsi_next_vobu & 0x7fffffff) {
            backupDlg->setProgress(sector);
            if (backupDlg->getAbort()) {
                seterror(tr2i18n("DVD backup cancelled"));
            }

            if (error) {
                vamps->abort();
                break;
            } else if (vamps->geterror()) {
                seterror( vamps->geterrMsg());
                break;
            }
            dsi_next_vobu= copyVobu(dvdfile,sector,NULL);
        }
    }
    vamps->setNoData();
    vamps->wait();
    uint32_t size=0;

    //JMP    size=m_position;
    size =lstCell->getPosition();

    dvdfile->close();
    outputFile->close();
    delete outputFile;
    outputFile=NULL;
    kifo.closeIFO();

    updateVob(lstCell);

    m_copyMenu=false;
    return size;
}



uint32_t k9DVDBackup::copyMenu(int _vts) {
    if (error)
        return 0;

    QFile *menuFile;
    QString targetName;
    if (_vts == 0) {
        targetName="VIDEO_TS.VOB";
    } else {
        targetName.sprintf("VTS_%02i_0.VOB",_vts);
    }

    uint32_t size;
    QString c;
    c="/VIDEO_TS/" + targetName;
    //gets the menu vob size
    k9Ifo kifo(&m_dvdread);
    kifo.openIFO( _vts);
    ifo_handle_t *hifo =kifo.getIFO();
    if (_vts==0)
        size=hifo->vmgi_mat->vmg_last_sector -1  - 2* hifo->vmgi_mat->vmgi_last_sector;
    else
        size=hifo->vtsi_mat->vtstt_vobs - hifo->vtsi_mat->vtsi_last_sector -1;
    kifo.closeIFO();

    size*=DVD_BLOCK_LEN;

    if (size ==0)
        return 0;

    menuFile=new QFile( output +"/"+targetName);
    if( !menuFile->open(IO_WriteOnly)) {
        seterror(tr2i18n("Unable to open ") +output +"/"+targetName);
        return 0;
    }

    k9DVDFile *dvdfile;
    if ((dvdfile = m_dvdread.openMenu( _vts))== 0) {
        QString stmp;
        stmp=i18n("Unable to open menu for titleset %1").arg(_vts);
        seterror (stmp);
        return 0;
    }


    size=size/DVD_VIDEO_LB_LEN;
    c=i18n("Extracting menu for titleset %1").arg(_vts);
    backupDlg->setProgressLabel(c);

    backupDlg->show();
    backupDlg->setTotalSteps(size);
    uchar buffer[DVD_VIDEO_LB_LEN];
    for (uint32_t sector=0 ;sector < size ;sector++) {
        backupDlg->setProgress(sector);
        if (backupDlg->getAbort())
            break;
        int len;
        len = dvdfile->readBlocks (sector, 1, buffer);
        menuFile->writeBlock((char*)buffer,DVD_VIDEO_LB_LEN);
        backupDlg->setProgressTotal(1);
    }

    dvdfile->close();
    menuFile->close();
    delete menuFile;

    return size;

}



void k9DVDBackup::playCell (int vts_num, k9Cell *_cell) {

    if (error)
        return;

    ifo_handle_t *vts_handle;
    k9DVDFile *dvdfile;
    uint32_t	sector, dsi_next_vobu = 0;
    /* open disc */
    if (m_dvdread.opened()) {
        /* load information for the given VTS */
        // vts_handle = ifoOpen (dvd_handle, vts_num);
        vts_handle=currTS->ifoTitle->getIFO();
        if (!vts_handle) {
            QString stmp;
            stmp=i18n("Unable to open ifo file for titleset %1").arg(vts_num);
            seterror (stmp);
            //JMP            vamps->setNoData();
            return;
        }

        backupDlg->setTotalSteps( vts_handle->vtsi_mat->vts_last_sector-vts_handle->vtsi_mat->vtstt_vobs  -1);
        QString c;
        c=i18n("Extracting titleset %1").arg(vts_num);
        backupDlg->setProgressLabel(c);
        backupDlg->show();
    } else {
        seterror(tr2i18n("Unable to open DVD"));
        //JMP        vamps->setNoData();
        return;
    }


    /* open VTS data */
    dvdfile = m_dvdread.openTitle (vts_num);
    if (! dvdfile) {
        QString stmp;
        stmp=i18n("Unable to open vobs for titleset %1").arg(vts_num);
        seterror( stmp);
        //JMP        vamps->setNoData();
        return;
    }


    /* loop until out of the cell */
    currCell->oldStartSector=_cell->startSector;
    for (sector = _cell->startSector;
            sector <= _cell->lastSector; sector += dsi_next_vobu & 0x7fffffff) {

        backupDlg->setProgress(sector);
        if (backupDlg->getAbort()) {
            seterror(tr2i18n("DVD backup cancelled"));
        }

        if (error) {
            vamps->abort();
            break;
        } else if (vamps->geterror()) {
            seterror( vamps->geterrMsg());
            break;
        }
        dsi_next_vobu= copyVobu(dvdfile,sector,NULL);
    }
    dvdfile->close();
}

void k9DVDBackup::setDummyNavPack(uchar *buf,uint32_t _sector)
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

  tc_memcpy (ptr, nav_pack1, sizeof (nav_pack1));
  ptr += sizeof (nav_pack1);
  memset (ptr, 0, DVD_VIDEO_LB_LEN/2 - sizeof (nav_pack1));
  ptr = (int8_t*)buf + DVD_VIDEO_LB_LEN/2;
  tc_memcpy (ptr, nav_pack2, sizeof (nav_pack2));
  ptr += sizeof (nav_pack2);
  memset (ptr, 0, DVD_VIDEO_LB_LEN/2 - sizeof (nav_pack2));

  dsi_t dsiPack;
  pci_t pciPack;
  navRead_DSI (&dsiPack, buf + DSI_START_BYTE);
  k9Ifo::navRead_PCI (&pciPack, buf+0x2d);
  dsiPack.dsi_gi.nv_pck_lbn=_sector;
  dsiPack.dsi_gi.vobu_ea = 0;

  navRead_DSI((dsi_t*)(buf + DSI_START_BYTE),(uchar*)&dsiPack);
  pciPack.pci_gi.nv_pck_lbn =dsiPack.dsi_gi.nv_pck_lbn;
  k9Ifo::navRead_PCI((pci_t*)(buf+0x2d),(uchar*)&pciPack);
}


void k9DVDBackup::setDummyPack(uchar *_buffer) {
    int8_t *ptr = (int8_t*)_buffer;
    uint8_t dummy_pack [] =
        {
            /* pack header: SCR=0, mux rate=10080000bps, stuffing length=0 */
            0, 0, 1, 0xba, 0x44, 0x00, 0x04, 0x00, 0x04, 0x01, 0x01, 0x89, 0xc3, 0xf8,
            /* PES header for dummy video packet */
            0, 0, 1, 0xe0, 0x07, 0xec, 0x81, 0x00, 0x00
        };

    tc_memcpy (ptr, dummy_pack, sizeof (dummy_pack));
    ptr += sizeof (dummy_pack);
    memset (ptr, 0xff, DVD_VIDEO_LB_LEN - sizeof (dummy_pack));

}


uint32_t k9DVDBackup::findNextVobu(uint32_t _sector) {
    k9Ifo ifo(&m_dvdread);
    ifo.openIFO(currVTS);
    m_ifo=ifo.getIFO();
    vobu_admap_t * vobu_admap;
    if (m_copyMenu)
	vobu_admap = m_ifo->menu_vobu_admap;
    else
        vobu_admap = m_ifo->vts_vobu_admap;
    uint32_t length = vobu_admap->last_byte + 1 - VOBU_ADMAP_SIZE;
    for(uint32_t i = 0; i < length/sizeof(uint32_t); i++) {
        if (vobu_admap->vobu_start_sectors[i]== _sector) {
	    uint32_t nextVobu=vobu_admap->vobu_start_sectors[i+1];
	    ifo.closeIFO();
            return nextVobu;
        }
    }
    ifo.closeIFO();
    return 0;

}



uint32_t k9DVDBackup::copyVobu(k9DVDFile  *_fileHandle,uint32_t _startSector,k9Vobu * _vobu) {
    dsi_t	dsi_pack;
    k9Vobu * currVobu;
    bool badNavPack=false;

    uint32_t	nsectors, len,nextVobu=0;
    uchar *buf;
    uint32_t sector=_startSector;
    /* read nav pack */
    buf=(uchar*) malloc(DVD_VIDEO_LB_LEN);
    len = _fileHandle->readBlocks ( sector, 1, buf);
    /* parse contained DSI pack */

    //test if nav pack is ok
    if (len !=-1) {
	navRead_DSI (&dsi_pack, buf + DSI_START_BYTE);
	if (dsi_pack.dsi_gi.nv_pck_lbn != sector) {
		len=-1;
	}
    }

    if (len==-1) {
       setDummyNavPack(buf,sector); 
       nextVobu=findNextVobu(sector);
       qDebug (QString("VOBU : %1 Read Error !!!!  ==>  %2").arg(sector).arg(nextVobu));
       badNavPack=true;
    }
    currVobu=_vobu;

    mutex.lock();
    if (k9Cell::isNavPack(buf)) {
        currCell->oldLastSector=sector;
        if (currVobu==NULL) {
            currVobu =currCell->addVobu(sector);
            vobuQueue.enqueue(currVobu);
        }
    }
    mutex.unlock();
    /* generate an MPEG2 program stream (including nav packs) */
    wrote=false;
    vamps->addData(buf,DVD_VIDEO_LB_LEN);
    m_inbytes+=DVD_VIDEO_LB_LEN;
    uint32_t end;

    if (badNavPack) {
       setDummyPack(buf);
       nsectors=1;
       if (nextVobu !=0) end=nextVobu-1;

    } else {
	nsectors      = dsi_pack.dsi_gi.vobu_ea;
	uint32_t dsi_next_vobu = dsi_pack.vobu_sri.next_vobu;
	
	buf=(uchar*) realloc(buf,nsectors*DVD_VIDEO_LB_LEN);
	
	end=nsectors;
	
	/* read VOBU */
	for (uint32_t i=0;i< nsectors;i++) {
		len = _fileHandle->readBlocks ( (sector + 1)+i, 1, buf +(i*DVD_VIDEO_LB_LEN));
		if (len==-1) {
		qDebug (QString("VOBU : %1 Read Error !!!!").arg(sector));
		//setDummyPack(buf + (i*DVD_VIDEO_LB_LEN));
		setDummyPack(buf);
		nsectors=1;
		break;
		}
	}
    }

    /* write VOBU */
    for (uint32_t i=0;i<nsectors ;i++) {
        vamps->addData(buf + (i*DVD_VIDEO_LB_LEN), DVD_VIDEO_LB_LEN);
    }
    free(buf);

    m_inbytes+=nsectors*DVD_VIDEO_LB_LEN;

    mutex.lock();
    qApp->processEvents();
    mutex.unlock();

    return (end+1);
}



k9Vobu * k9DVDBackup::remapVobu(uint32_t *value) {
    k9Vobu *vobu=NULL;
    uint32_t sector,mask;
    if ( (*value & 0x80000000) == 0x80000000) {
        sector = *value & 0x7FFFFFFF;
        mask=0x80000000;
    } else {
        sector =*value;
        mask=0;
    }
    *value=0;
    k9CellList *lstCell;
    if(! m_copyMenu) {
        lstCell=&(currTS->cells);
    } else {
        if (currVTS==0)
            lstCell=&vmgCells;
        else
            lstCell=&(currTS->menuCells);
    }
    vobu=lstCell->findVobu(sector);
    if (vobu !=NULL) {
        *value = vobu->newSector | mask;
        return vobu;
    }

    *value=0;
    return vobu;

}


k9Vobu * k9DVDBackup::remapOffset(uint32_t _sector,uint32_t *_offset,int _dir) {
    k9Vobu *vobu1=NULL, *vobu2=NULL;
    uint32_t offset,sector;
    uint32_t maskOffset1=0,maskOffset2=0,maskSector=0;

    if ((*_offset!= 0xbfffffff) && (*_offset!=0x3fffffff) && (*_offset!=0x7fffffff)) {

        if ( (*_offset & 0x80000000) == 0x80000000)
            maskOffset1= 0x80000000;
        if ( (*_offset & 0x40000000) == 0x40000000)
            maskOffset2= 0x40000000;
        offset = *_offset & 0x3FFFFFFF;

        if ( (_sector & 0x80000000) == 0x80000000) {
            sector = _sector & 0x7FFFFFFF;
            maskSector=0x80000000;
        } else {
            sector =_sector;
            maskSector=0;
        }

        k9CellList *lstCell;
        if(! m_copyMenu) {
            lstCell=&(currTS->cells);
        } else {
            if (currVTS==0)
                lstCell=&vmgCells;
            else
                lstCell=&(currTS->menuCells);
        }
        vobu1 = lstCell->findVobu(sector);
        vobu2 = lstCell->findVobu(sector+_dir*offset);



        if ((vobu1 !=NULL) && (vobu2!=NULL)) {
            *_offset = abs(vobu1->newSector - vobu2->newSector)  | maskOffset1 ;
            *_offset |= maskOffset2;
            return vobu2;
        }

        if (vobu1==NULL && vobu2==NULL)
            qDebug ("remapOffset : sector not found");
    }
    return vobu2;
}


void k9DVDBackup::updateMainIfo() {

    if (error)
        return;

    k9Ifo ifo(&m_dvdread);
    ifo.setOutput(output);
    ifo.setDevice(device);

    ifo.openIFO(0);
    ifo_handle_t *hifo =ifo.getIFO();

    hifo->vmgi_mat->vmg_last_sector= 1+ m_vmgSize+hifo->vmgi_mat->vmgi_last_sector*2;
    if (m_vmgSize >0) {
        if (hifo->vmgi_mat->vmgm_vobs != hifo->vmgi_mat->vmgi_last_sector +1)
            qDebug (QString("error in ifo file : vmgm_vobs %1 -> %2)").arg(hifo->vmgi_mat->vmgm_vobs).arg(hifo->vmgi_mat->vmgi_last_sector +1));
        hifo->vmgi_mat->vmgm_vobs=hifo->vmgi_mat->vmgi_last_sector +1;
    }

    currVTS=0;

    k9Vobu* vobu2=NULL;
    uint32_t newPos=0;
    //update first play PGC
    if (hifo->first_play_pgc !=NULL ) {
	m_copyMenu=true;
        pgc_t *pgc=hifo->first_play_pgc;
        cell_playback_t *cell_playback =pgc->cell_playback;
        int nr= pgc->nr_of_cells;

        vobu2=NULL;
        cell_playback_t cell;
        newPos=0;
        for( int j = 0; j < nr; j++) {
            k9Vobu *vobu=remapVobu(&cell_playback[j].first_sector);
            vobu2=vobu;

            remapVobu(&cell_playback[j].first_ilvu_end_sector);
            if (vobu !=NULL) {
                vobu=remapVobu(&cell_playback[j].last_vobu_start_sector);
                if (vobu==NULL) {
                    cell_playback[j].last_vobu_start_sector=cell_playback[j].first_sector;
                    vobu=vobu2;
                    pgc->playback_time.hour=0;
                    pgc->playback_time.minute=0;
                    pgc->playback_time.second=0;

                    cell_playback[j].playback_time.hour=0;
                    cell_playback[j].playback_time.minute=0;
                    cell_playback[j].playback_time.second=0;
                }
                cell_playback[j].last_sector = vobu->newSector+vobu->size;// -1 ;
                cell_playback[newPos]=cell_playback[j];
                cell=cell_playback[newPos];
                newPos++;
            } else {
                cell_playback[newPos]=cell;
                newPos++;
            }
        }
        for (uint32_t j=newPos;j<nr;j++)
            cell_playback[j].last_sector=0;
        pgc->nr_of_cells=newPos;
	m_copyMenu=false;
    }



    updatePgci_ut(hifo);
    update4Menu(hifo);

    //mise �jour des startSectors
    k9TitleSet *TSp=NULL;
    titleSets.sort();
    for (uint iTS=0;iTS < titleSets.count();iTS++) {
        k9TitleSet *TS=titleSets.at(iTS);
        uint32_t startSector;
        if (TSp!=NULL)
            startSector = TSp->startSector + TSp->getSize();
        else
            startSector=hifo->vmgi_mat->vmg_last_sector+1  ;
        TS->startSector=startSector;
        TSp=TS;
    }

    hifo->vmgi_mat->vmg_category=0;
    bool found=false;
    for (uint32_t i=0 ; i< hifo->tt_srpt->nr_of_srpts;i++) {
        for (uint iTS=0;iTS<titleSets.count() &&(!found);iTS++) {
            k9TitleSet *TS=titleSets.at(iTS);
            if (TS->VTS == hifo->tt_srpt->title[i].title_set_nr ) {
                hifo->tt_srpt->title[i].title_set_sector = TS->startSector;
                found=true;
            }
        }
        found=false;
    }

    ifo.saveIFO();
    ifo.closeIFO();

}

void k9DVDBackup::updatePgci_ut(ifo_handle_t *_hifo) {
    pgci_ut_t *pgci_ut=_hifo->pgci_ut;
    k9Vobu* vobu2=NULL;
    uint newPos=0;
    //update pgci_ut

    if (pgci_ut !=0) {
        m_copyMenu=true;
        for (uint i=0; i <pgci_ut->nr_of_lus;i++) {
            pgci_lu_t *pgci_lu= &pgci_ut->lu[i];
            pgcit_t * pgcit= pgci_lu->pgcit;
            for (uint i2=0 ; i2 <pgcit->nr_of_pgci_srp;i2++) {
                pgci_srp_t * pgci_srp=&pgcit->pgci_srp[i2];
                pgc_t *pgc=pgci_srp->pgc;

                cell_playback_t *cell_playback =pgc->cell_playback;
                int nr= pgc->nr_of_cells;

                vobu2=NULL;
                cell_playback_t cell;
                newPos=0;
                for( int j = 0; j < nr; j++) {
                    k9Vobu *vobu=remapVobu(&cell_playback[j].first_sector);
                    vobu2=vobu;

                    if (cell_playback[j].first_ilvu_end_sector !=0) {
                        uint32_t tmp=cell_playback[j].first_ilvu_end_sector+1;
                        remapVobu(&tmp);
                        if (tmp!=0)
                            cell_playback[j].first_ilvu_end_sector=tmp-1;
                    }
                    if (vobu !=NULL) {
                        vobu=remapVobu(&cell_playback[j].last_vobu_start_sector);
                        cell_playback[j].last_sector = vobu->newSector+vobu->size;// -1 ;
                        cell_playback[newPos]=cell_playback[j];
                        cell=cell_playback[newPos];
                        newPos++;
                    } else {
                        cell_playback[newPos]=cell;
                        newPos++;
                    }
                }
                for (uint32_t j=newPos;j<nr;j++)
                    cell_playback[j].last_sector=0;
                pgc->nr_of_cells=newPos;
            }
        }
        m_copyMenu=false;
    }


}

void k9DVDBackup::update4Menu(ifo_handle_t *_hifo) {
    // Mise �jour vtsm_c_adt pour le menu
    m_copyMenu=true;   //indispensable pour remapvobu
    c_adt_t *c_adt = _hifo->menu_c_adt;
    uint32_t length;
    if (c_adt!=NULL) {
        length = c_adt->last_byte + 1 - C_ADT_SIZE;
        cell_adr_t *ptr= c_adt->cell_adr_table;
        for(uint32_t i = 0; i < length/sizeof(cell_adr_t); i++) {
            uint32_t startSect=ptr[i].start_sector;
            // last sector of a vobu = start sector of next vobu -1
            uint32_t lastSect= ptr[i].last_sector +1;
            k9Vobu *vobu=remapVobu(&startSect);
            if (vobu == NULL)
                qDebug ("Error : could not find startSector");
            else  {
                if (remapVobu(&lastSect)==NULL)
                    lastSect=vobu->parent->lastSector;
                else
                    lastSect--;
                ptr[i].start_sector = startSect;
                ptr[i].last_sector = lastSect;
            }

        }
    }

    vobu_admap_t * vobu_admap = _hifo->menu_vobu_admap;
    if (vobu_admap != NULL) {
        length = vobu_admap->last_byte + 1 - VOBU_ADMAP_SIZE;
        uint newPos=0;
        for(uint32_t i = 0; i < length/sizeof(uint32_t); i++) {
            if (remapVobu(&vobu_admap->vobu_start_sectors[i])!= NULL) {
                vobu_admap->vobu_start_sectors[newPos]=vobu_admap->vobu_start_sectors[i];
                newPos++;
            }
        }
        for (uint32_t i=newPos ; i < length/sizeof(uint32_t);i++)
            vobu_admap->vobu_start_sectors[i]=0;

        vobu_admap->last_byte = newPos * sizeof(uint32_t) -1 +VOBU_ADMAP_SIZE;
    }
    m_copyMenu=false;

}





void k9DVDBackup::updateIfo() {

    if (error)
        return;

    k9Ifo ifo(&m_dvdread);
    ifo.setOutput(output);
    ifo.setDevice(device);

    ifo.openIFO(currVTS);

    ifo_handle_t *hifo =ifo.getIFO();

    pgcit_t * pgcit = hifo->vts_pgcit;

    //update total VTS size with IFO size 
   currTS->lastSector += 2 *(hifo->vtsi_mat->vtsi_last_sector ) +1;

    hifo->vtsi_mat->vts_last_sector =   currTS->lastSector ;

    hifo->vtsi_mat->vtstt_vobs = hifo->vtsi_mat->vtsi_last_sector + 1 + currTS->menuSize;
    //JMP
    if (currTS->menuSize >0) {
        if (hifo->vtsi_mat->vtsm_vobs != hifo->vtsi_mat->vtsi_last_sector +1)
            qDebug (QString("error in ifo file %3 : vtsm_vobs %1 -> %2").arg(hifo->vtsi_mat->vtsm_vobs).arg(hifo->vtsi_mat->vtsi_last_sector +1).arg(currTS->VTS));
        hifo->vtsi_mat->vtsm_vobs= hifo->vtsi_mat->vtsi_last_sector +1 ;

    }

    updatePgci_ut(hifo);

    k9Vobu* vobu2=NULL;
    uint32_t newPos=0;
    //update first play PGC
    if (hifo->first_play_pgc !=NULL ) {
        pgc_t *pgc=hifo->first_play_pgc;
        cell_playback_t *cell_playback =pgc->cell_playback;
        int nr= pgc->nr_of_cells;

        vobu2=NULL;
        cell_playback_t cell;
        newPos=0;
        for( int j = 0; j < nr; j++) {
            k9Vobu *vobu=remapVobu(&cell_playback[j].first_sector);
            vobu2=vobu;

            remapVobu(&cell_playback[j].first_ilvu_end_sector);
            if (vobu !=NULL) {
                vobu=remapVobu(&cell_playback[j].last_vobu_start_sector);
                if (vobu==NULL) {
                    cell_playback[j].last_vobu_start_sector=cell_playback[j].first_sector;
                    vobu=vobu2;
                    pgc->playback_time.hour=0;
                    pgc->playback_time.minute=0;
                    pgc->playback_time.second=0;

                    cell_playback[j].playback_time.hour=0;
                    cell_playback[j].playback_time.minute=0;
                    cell_playback[j].playback_time.second=0;
                }
                cell_playback[j].last_sector = vobu->newSector+vobu->size;// -1 ;
                cell_playback[newPos]=cell_playback[j];
                cell=cell_playback[newPos];
                newPos++;
            } else {
                cell_playback[newPos]=cell;
                newPos++;
            }
        }
        for (uint32_t j=newPos;j<nr;j++)
            cell_playback[j].last_sector=0;
        pgc->nr_of_cells=newPos;
    }

    newPos=0;
    //update each PGC


    for(uint32_t i = 0; i < pgcit->nr_of_pgci_srp; i++) {
        pgc_t *pgc=pgcit->pgci_srp[i].pgc;
        cell_playback_t *cell_playback =pgc->cell_playback;
        int nr= pgc->nr_of_cells;

        vobu2=NULL;
        cell_playback_t cell;
        newPos=0;
        for( int j = 0; j < nr; j++) {
            k9Vobu *vobu=remapVobu(&cell_playback[j].first_sector);
            vobu2=vobu;

            if (cell_playback[j].first_ilvu_end_sector !=0) {
                uint32_t tmp=cell_playback[j].first_ilvu_end_sector+1;
                remapVobu(&tmp);
                if (tmp!=0)
                    cell_playback[j].first_ilvu_end_sector=tmp-1;
            }
            if (vobu !=NULL) {
                vobu=remapVobu(&cell_playback[j].last_vobu_start_sector);
                if (vobu==NULL) {
                    cell_playback[j].last_vobu_start_sector=cell_playback[j].first_sector;
                    vobu=vobu2;
                    pgc->playback_time.hour=0;
                    pgc->playback_time.minute=0;
                    pgc->playback_time.second=0;

                    cell_playback[j].playback_time.hour=0;
                    cell_playback[j].playback_time.minute=0;
                    cell_playback[j].playback_time.second=0;

                }
                cell_playback[j].last_sector = vobu->newSector+vobu->size;// -1 ;
                cell_playback[newPos]=cell_playback[j];
                cell=cell_playback[newPos];
                newPos++;
            } else {
                cell_playback[newPos]=cell;
                newPos++;
            }
        }
        for (uint32_t j=newPos;j<nr;j++)
            cell_playback[j].last_sector=0;
        pgc->nr_of_cells=newPos;
    }

    c_adt_t *c_adt = hifo->vts_c_adt;
    uint32_t length = c_adt->last_byte + 1 - C_ADT_SIZE;
    cell_adr_t *ptr= c_adt->cell_adr_table;
    newPos=0;
    for(uint32_t i = 0; i < length/sizeof(cell_adr_t); i++) {
        uint32_t startSect=ptr[i].start_sector;
        // last sector of a vobu = start sector of next vobu -1
        uint32_t lastSect= ptr[i].last_sector +1;
        k9Vobu *vobu=remapVobu(&startSect);
        if (vobu == NULL)
            qDebug ("Error : could not find startSector");
        else {
            if (remapVobu(&lastSect)==NULL)
                lastSect= vobu->parent->lastSector;
            else
                lastSect--;
            ptr[i].start_sector = startSect;
            ptr[i].last_sector = lastSect;
        }
    }


    vobu_admap_t * vobu_admap = hifo->vts_vobu_admap;
    length = vobu_admap->last_byte + 1 - VOBU_ADMAP_SIZE;
    newPos=0;
    for(uint32_t i = 0; i < length/sizeof(uint32_t); i++) {
        if (remapVobu(&vobu_admap->vobu_start_sectors[i])!= NULL) {
            vobu_admap->vobu_start_sectors[newPos]=vobu_admap->vobu_start_sectors[i];
            newPos++;
        }
    }
    for (uint32_t i=newPos ; i < length/sizeof(uint32_t);i++)
        vobu_admap->vobu_start_sectors[i]=0;

    vobu_admap->last_byte = newPos * sizeof(uint32_t) -1 +VOBU_ADMAP_SIZE;


    //update VTS_TMAP
    vts_tmapt_t *vts_tmapt=hifo->vts_tmapt;
    if (vts_tmapt) {
        for(uint32_t i = 0; i < vts_tmapt->nr_of_tmaps; i++) {
            if(vts_tmapt->tmap[i].nr_of_entries == 0) { // Early out if zero entries
                continue;
            }
            map_ent_t * map_ent=vts_tmapt->tmap[i].map_ent;
            newPos=0;
            for(uint32_t j = 0; j < vts_tmapt->tmap[i].nr_of_entries; j++) {
                //bit 31 indicates whether  VOBU time codes are discontinous with previous
                uint32_t mask=map_ent[j] & 0x80000000 ;
                uint32_t value=map_ent[j] & 0x7FFFFFFF;
                if (remapVobu(&value) !=NULL) {
                    map_ent[j]=value | mask;
                    map_ent[newPos]=map_ent[j];
                    newPos++;
                } else
                    map_ent[j]=0;
            }
            for (int j = newPos; j < vts_tmapt->tmap[i].nr_of_entries;j++)
                map_ent[j]=0;
            vts_tmapt->tmap[i].nr_of_entries=newPos;
        }
    }

    update4Menu(hifo);

    ifo.saveIFO();
}

void k9DVDBackup::updateVob(k9CellList *cellLst) {
    int nbVobuUpdated=0;

    uchar buffer[DVD_VIDEO_LB_LEN];
    FILE *file=NULL;
    QString dbg;
    int pVobNum=-1;
    //for (uint iCell=0;iCell<currTS->cells.count();iCell++) {
    //    k9Cell *cell=currTS->cells.at(iCell);
    for (uint iCell=0;iCell< cellLst->count();iCell++) {
        k9Cell *cell=cellLst->at(iCell);
        for (uint ivobu=0; ivobu<cell->vobus.count();ivobu++) {
            qApp->processEvents();
            k9Vobu * vobu = cell->vobus.at(ivobu);
            int VobNum=vobu->vobNum;
            if (error)
                return;
            if (pVobNum !=VobNum) {
                if (file !=NULL)
                    fclose(file);
                QString sName;
                if (currVTS==0)
                    sName="VIDEO_TS.VOB";
                else
                    sName.sprintf("VTS_%02d_%d.VOB",currVTS,VobNum);
                dbg=i18n("Updating vob %1").arg(sName);
                sName=output+"/"+sName;
                QFileInfo finfo(sName);
                long fileSize=finfo.size();

                backupDlg->setTotalSteps(fileSize);
                backupDlg->setProgressLabel(dbg);

                file=fopen(sName,"r+b");
                pVobNum=VobNum;
            }
            if( file !=NULL) {
                uint32_t sector=0;
                long pos=vobu->vobPos;
                backupDlg->setProgress(pos);
                if (backupDlg->getAbort()) {
                    seterror(tr2i18n("DVD backup canceled"));
                    break;
                }

                bool emptyPgc=false;
                /*		long currpos=ftell(file);
                		long offset=pos-currpos;
                		fseek(file,offset,SEEK_CUR);
                */
                fseek(file,pos,SEEK_SET);
                fread(buffer,DVD_VIDEO_LB_LEN,1,file);
                if (k9Cell::isNavPack((uchar*)buffer)) {
                    dsi_t dsiPack;
                    pci_t pciPack;
                    nbVobuUpdated++;
                    navRead_DSI (&dsiPack, buffer + DSI_START_BYTE);
                    k9Ifo::navRead_PCI (&pciPack, buffer+0x2d);
                    sector=dsiPack.dsi_gi.nv_pck_lbn;  //JMP : pour debug
                    //vobu=remapVobu(&dsiPack.dsi_gi.nv_pck_lbn );
                    sector=vobu->oldSector;
                    dsiPack.dsi_gi.nv_pck_lbn=vobu->newSector;
                    if (vobu != NULL) {
                        dsiPack.dsi_gi.vobu_ea = vobu->size;
                        emptyPgc=vobu->empty;
                    } else {
                        dbg.sprintf("remapVobu failed for %d",dsiPack.dsi_gi.nv_pck_lbn);
                    }

                    if (!emptyPgc) {
                        remapOffset(sector, &dsiPack.vobu_sri.next_video,1 );
                        for (int i =0;i<19;i++) {
                            remapOffset(sector,&dsiPack.vobu_sri.fwda[i],1);
                        }
                        remapOffset(sector,&dsiPack.vobu_sri.next_vobu,1);
                        remapOffset(sector,&dsiPack.vobu_sri.prev_vobu,-1);
                        for (int i =0;i<19;i++) {
                            remapOffset(sector,&dsiPack.vobu_sri.bwda[i],-1);
                        }

                        remapOffset(sector,&dsiPack.vobu_sri.prev_video,-1);

                        //1st audio packet
                        for (int i =0 ;i <8 ;i++) {
                            if (((dsiPack.synci.a_synca[i] & 0x8000) != 0x8000 ) && (dsiPack.synci.a_synca[i] !=0x3FFF)) {
                                if (vobu->firstAudio[i] !=-1) {
                                    dsiPack.synci.a_synca[i]=vobu->firstAudio [i];
                                } else {
                                    dsiPack.synci.a_synca[i] =0;
                                }
                            }
                        }
                        //1st subpicture packet
                        for (int i =0 ;i <32 ;i++) {
                            if (((dsiPack.synci.sp_synca[i] & 0x80000000) != 0x80000000) &&
                                    (dsiPack.synci.sp_synca[i] != 0x3FFFFFFF) && (dsiPack.synci.sp_synca[i] != 0x7FFFFFFF)) {
                                if (vobu->firstSubp[i] !=-1) {
                                    dsiPack.synci.sp_synca[i]=vobu->firstSubp [i];
                                } else {
                                    dsiPack.synci.sp_synca[i] =0;
                                }
                            }
                        }
                        //ILVU

                        for (int i=0;i<9;i++) {
                            if (dsiPack.sml_agli.data[i].address !=0 && dsiPack.sml_agli.data[i].address !=0x7FFFFFFF) {
                                uint32_t tmpAdr=dsiPack.sml_agli.data[i].address;
                                uint32_t tmpSize=(dsiPack.sml_agli.data[i].address & 0x7FFFFFFF)  + dsiPack.sml_agli.data[i].size;
                                int dir;
                                if ((tmpAdr & 0x80000000) ==0x80000000)
                                    dir=-1;
                                else
                                    dir=1;
                                remapOffset(sector,&tmpAdr,dir);
                                remapOffset(sector,&tmpSize,1);
                                dsiPack.sml_agli.data[i].address=tmpAdr;
                                dsiPack.sml_agli.data[i].size=tmpSize-(tmpAdr &0x7FFFFFFF);
                            }
                        }


                        if (dsiPack.sml_pbi.ilvu_ea !=0) {
                            uint32_t tmp=dsiPack.sml_pbi.ilvu_ea+1;
                            remapOffset(sector,&tmp,1);
                            if (tmp!=0)
                                tmp--;
                            dsiPack.sml_pbi.ilvu_ea=tmp;
                        }
                        if (dsiPack.sml_pbi.ilvu_sa !=0) {
                            k9Vobu *vobu2=remapOffset(sector,&dsiPack.sml_pbi.ilvu_sa,1);
                            if (vobu2!= NULL) {
                                FILE *file2;
                                if ( vobu2->vobNum != VobNum) {
                                    QString sName;
                                    sName.sprintf("/VTS_%02d_%d.VOB",currVTS,vobu2->vobNum);
                                    sName=output+sName;
                                    file2=fopen(sName,"rb");
                                } else
                                    file2=file;
                                fseek(file2,vobu2->vobPos,SEEK_SET);
                                uchar *tmpbuff=(uchar*)malloc(2048);
                                fread(tmpbuff,DVD_VIDEO_LB_LEN,1,file2);
                                dsi_t dsiNext;
                                navRead_DSI (&dsiNext, tmpbuff + DSI_START_BYTE);
                                uint32_t sectmp= dsiNext.sml_pbi.ilvu_ea+1;
                                remapOffset(dsiNext.dsi_gi.nv_pck_lbn,&sectmp,1);
                                dsiPack.sml_pbi.size=sectmp;
                                free (tmpbuff);
                                if (vobu2->vobNum!=VobNum)
                                    fclose(file2);
                            }
                        }
                        // end block reference frames
                        dsiPack.dsi_gi.vobu_1stref_ea = vobu->firstRef;
                        dsiPack.dsi_gi.vobu_2ndref_ea=vobu->secondRef;
                        dsiPack.dsi_gi.vobu_3rdref_ea=vobu->thirdRef;


                        // update pci pack
                        for (int i=0; i<9;i++) {
                            if ((pciPack.nsml_agli.nsml_agl_dsta[i] & 0x80000000) != 0x80000000)
                                remapOffset(sector,&pciPack.nsml_agli.nsml_agl_dsta[i],1);
                        }

                    } else {
                        dsiPack.vobu_sri.next_video= 0xbfffffff;
                        for (int i =0;i<19;i++)
                            dsiPack.vobu_sri.fwda[i] = 0x3fffffff;
                        dsiPack.vobu_sri.next_vobu=0x3fffffff;
                        dsiPack.vobu_sri.prev_vobu=0x3fffffff;
                        for (int i =0;i<19;i++)
                            dsiPack.vobu_sri.bwda[i] = 0x3fffffff;
                        dsiPack.vobu_sri.prev_video=0xbfffffff;
                        for (int i =0 ;i <8 ;i++)
                            dsiPack.synci.a_synca[i]=0;
                        for (int i =0 ;i <32 ;i++)
                            dsiPack.synci.sp_synca[i] =0;
                        // end block reference frames
                        dsiPack.dsi_gi.vobu_1stref_ea = 0;
                        dsiPack.dsi_gi.vobu_2ndref_ea=0;
                        dsiPack.dsi_gi.vobu_3rdref_ea=0;
                        //JMP for tests
                        pciPack.pci_gi.vobu_s_ptm=0;
                        pciPack.pci_gi.vobu_e_ptm=0;
                        pciPack.pci_gi.vobu_se_e_ptm=0;
                        pciPack.pci_gi.e_eltm.hour = pciPack.pci_gi.e_eltm.minute =pciPack.pci_gi.e_eltm.second=0;
                        dsiPack.dsi_gi.c_eltm.hour=dsiPack.dsi_gi.c_eltm.minute=dsiPack.dsi_gi.c_eltm.second=0;
                    }
                    // mise en place des donnees modifi�s dans le buffer de sortie
                    navRead_DSI((dsi_t*)(buffer + DSI_START_BYTE),(uchar*)&dsiPack);
                    pciPack.pci_gi.nv_pck_lbn =dsiPack.dsi_gi.nv_pck_lbn;
                    k9Ifo::navRead_PCI((pci_t*)(buffer+0x2d),(uchar*)&pciPack);
                    //mise �jour du fichier
                    fseek(file,pos,SEEK_SET);
                    fwrite(buffer,DVD_VIDEO_LB_LEN,1,file);
                }

            } else {
                qDebug ("erreur positionning");
            }
        }
    }
    if (file!=NULL)
        fclose(file);
}

void k9DVDBackup::clearOutput(QString name) {
    QDir dir(name);
    //delete files in directory
    QStringList lst = dir.entryList( "*",QDir::Files |QDir::Hidden );
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        QString c(( *it ).latin1() );
        dir.remove (c);
    }
    //scanning subdir
    QStringList lstdir = dir.entryList( "*",QDir::Dirs );
    for ( QStringList::Iterator it = lstdir.begin(); it != lstdir.end(); ++it ) {
        QString c(( *it ).latin1() );
        if ((c!=".") && c!="..") {
            clearOutput(dir.absFilePath(c));
            dir.rmdir(c);
        }
    }

}

uint k9DVDBackup::getLastCell(k9CellCopyList *_cellCopyList, uint _index) {

    k9Cell *orig=(k9Cell*)_cellCopyList->at(_index);
    uint result=_index;

    uint64_t dsize=1+orig->lastSector-orig->startSector;

    uchar audioOrig[8];
    uchar subpOrig[32];
    memset(audioOrig,0,sizeof(uchar)*8);
    memset(subpOrig,0,sizeof(uchar)*32);

    ;
    QValueList<int>::iterator it;
    for ( it = orig->audio.begin(); it != orig->audio.end(); ++it )
        audioOrig[*it -1]=1;

    for ( it = orig->subpicture.begin(); it != orig->subpicture.end(); ++it )
        subpOrig[*it -1]=1;

    for (uint iCell=_index+1;(iCell<_cellCopyList->count()) ;iCell++) {
        k9Cell *cell=(k9Cell*)_cellCopyList->at(iCell);
        if ((cell->vts== orig->vts) && ( cell->selected)) {

            uchar audio[8];
            uchar subp[32];
            memset(audio,0,sizeof(uchar)*8);
            memset(subp,0,sizeof(uchar)*32);
            ;
            QValueList<int>::iterator it;
            for ( it = cell->audio.begin(); it != cell->audio.end(); ++it )
                audio[*it -1]=1;

            for ( it = cell->subpicture.begin(); it != cell->subpicture.end(); ++it )
                subp[*it -1]=1;

            if ( (memcmp(audioOrig,audio,sizeof(uchar)*8) ==0) && (memcmp(subpOrig,subp,sizeof(uchar)*32) ==0))  {
                result=iCell;
                dsize+=1+cell->lastSector-cell->startSector;
            } else
                break;
        } else
            break;
    }

    dsize*=DVD_BLOCK_LEN;
    argSize=dsize;
    return result;
}

void k9DVDBackup::calcFactor() {
    double factor=m_cellCopyList->getfactor(true,false,m_inbytes,m_outbytes);
    QString sFactor;
    sFactor.sprintf("%.2f",factor);
    backupDlg->setFactor(sFactor);
    argFactor =  factor;
}

void k9DVDBackup::execute() {
    QString sOutput=output;

    output=QDir::cleanDirPath(output +"/dvd");

    QDir root("/");
    root.mkdir(output);
    clearOutput(output);

    QDir dir(output);
    dir.mkdir("VIDEO_TS");
    dir.mkdir("AUDIO_TS");

    output=QDir::cleanDirPath(output +"/VIDEO_TS");

    m_dvdread.openDevice(device);
    if (!m_dvdread.opened()) {
        seterror(tr2i18n("Unable to open DVD"));
        return;
    }

    k9CellCopyList *cellCopyList =new k9CellCopyList(&m_dvdread,DVD);
    m_cellCopyList=cellCopyList;

    double totalSize=DVD->getmenuSize() *2048 ;
    totalSize+=cellCopyList->gettotalSize();
    totalSize/=(1024*1024);
    totalSize = (totalSize >k9DVDSize::getMaxSize()) ? k9DVDSize::getMaxSize():totalSize;

    backupDlg->setTotalMax(totalSize);
    m_inbytes=m_outbytes=0;

    int lastCell;
    calcFactor();

    //VTSList is sorted by size, so it is easyer to ajust the compression factor
    for(uint iTS=0;iTS<cellCopyList->VTSList.count() &&(!error);iTS++) {
        k9CellCopyVTS *VTS=cellCopyList->VTSList.at(iTS);
        //loop on each cell from the titleset
        lastCell=-1;
        for (int iCell=0;(iCell<cellCopyList->count()) ;iCell++) {
            k9Cell *cell=(k9Cell*)cellCopyList->at(iCell);
            if (cell->vts== VTS->getnum() && (!cell->copied)) {
                //		currCopyCell=cell;
                if (lastCell <iCell) {
                    lastCell=getLastCell( cellCopyList,iCell);
                    //adjusting factor of compression
                    calcFactor();
                }

                copyCell(cell->vts,cell,! cell->selected);
                if (!error) {
                    cell->copied=true;
                }
                //                }
                if (lastCell==iCell) {
                    vamps->setNoData();
                    vamps->wait();
                }
                if (error)
                    break;
            }
        }
    }
    delete cellCopyList;

    if (!error) {
        updateIfo();
        updateVob(&currTS->cells);
        updateMainIfo();
    }
    output=sOutput;
    backupDlg->hide();

    if (error)
        KMessageBox::error(0,errMsg,"DVD Backup");
    m_dvdread.close();
}
