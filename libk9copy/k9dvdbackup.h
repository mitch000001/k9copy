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
#ifndef K9DVDBACKUP_H
#define K9DVDBACKUP_H

#include "k9common.h"
#include "k9cell.h"
#include "k9cellcopylist.h"
#include "k9dvdread.h"
#include <qptrqueue.h>
#include <qmutex.h>
#include <qfile.h>
/**
@author Jean-Michel Petit
*/

class k9BackupDlg;
class k9Ifo2;

class k9TitleSet : public QObject {
    Q_OBJECT
public:
    k9TitleSet(int _VTS);
    ~k9TitleSet();
    uint32_t startSector;
    uint32_t lastSector;
    int VTS;
	uint32_t menuSize;
    k9CellList cells,menuCells;
    k9Cell*addCell(int _vts,int _pgc, int _vob);
    uint32_t getSize();
    k9Ifo2 *ifoTitle;
private:
};

class k9TitleSetList:public QPtrList <k9TitleSet> {
protected:
	 int compareItems ( QPtrCollection::Item item1, QPtrCollection::Item item2 );
};

class k9CadtList:public QPtrList<cell_adr_t> {
protected:
   int compareItems(QPtrCollection::Item item1,QPtrCollection::Item item2);
};

class k9vamps;

class k9DVDBackup : public QObject {
    Q_OBJECT
public:
    k9DVDBackup(QObject  *_dvd,const char* name=0,const QStringList& args=0) ;

    ~k9DVDBackup();
    virtual void setDevice(QString _device);
    virtual void setOutput(QString _output);
    virtual QString getErrMsg();
    virtual void execute();
    virtual bool geterror();
    virtual void setWithMenus(bool _value);
public  slots:
    void getOutput(uchar *buffer,uint32_t size);

protected:
    QString device;
    QString output;
    bool wrote;
private:
    k9DVDRead *m_dvdread;
    bool m_copyMenu;
    bool m_withMenu;
    bool m_forcedFactor;
    k9DVD *DVD;
    QString errMsg;
    QString vampsMsg;
    QFile* outputFile;
    long cellSize;
    long currVTS;
    k9TitleSet *currTS;
    int currVOB;
    ifo_handle_t *m_ifo;
    k9Cell *currCell,*cellOut;//,*currCopyCell;
    k9CellCopyList *m_cellCopyList;
    uint32_t m_position,m_vmgSize;
    //uint64_t m_inbytes,m_outbytes;
    k9TitleSetList titleSets;
    float argFactor;
    uint64_t argSize;
    void prepareVTS(int VTS);
    void playCell (int vts_num, k9Cell *_cell,bool _empty=false);
    uint32_t findNextVobu(uint32_t _sector);
    uint32_t copyVobu(k9DVDFile  *_fileHandle,uint32_t _startSector,k9Vobu *_vobu,bool _empty=false);
    void copyCell(int VTS, k9Cell *_cell,bool _empty);
//    void copyAngleBlock(k9CellCopyList *_list,uint _num);
    void copyEmptyPgc(int _vts,k9Cell *_cell);
    void updateVob(k9CellList *_cells);
    
    void updateIfo();
    void updateMainIfo();
    void updatePgci_ut(ifo_handle_t *_hifo);
    void update4Menu(ifo_handle_t *_hifo);
    void setDummyPack(uchar *_buffer);
    void setDummyNavPack(uchar *_buffer,uint32_t _sector);
    uint32_t copyMenu2(int _vts);
    k9CellList vmgCells;

    k9Vobu * remapVobu(uint32_t *value);
    k9Vobu * remapOffset(uint32_t _sector,uint32_t *offset,int dir);
    k9BackupDlg *backupDlg;
    void clearOutput(QString name);
    uint  getLastCell(k9CellCopyList *_cellCopyList,uint _index);
    void calcFactor();
    void forceFactor(double _factor);
    bool error;
    void seterror(const QString &_msg);
    k9vamps *vamps;
    QPtrQueue <k9Vobu> vobuQueue;
    QMutex mutex;
};



#endif
