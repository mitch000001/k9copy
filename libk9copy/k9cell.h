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
#ifndef K9CELL_H
#define K9CELL_H

#include "k9common.h"
#include "k9dvdtitle.h"


enum streamType_t {stAudio,stVideo,stSubpicture,stOther};
/**
@author Jean-Michel Petit
*/
class k9CellList;
class k9Cell;
class k9Vobu : public QObject {
    Q_OBJECT
public:
    k9Vobu(k9Cell * parent,uint32_t _oldSector);
    ~k9Vobu();
    uint32_t newSector;
    uint32_t oldSector;
    uint32_t size;
    int firstAudio[8];
    int firstSubp[32];
    int firstVideo;
    bool empty;
    k9Cell *parent;
    long vobPos;
    int vobNum;
    uchar frameType;
    uint32_t  firstRef,secondRef,thirdRef;
    bool firstRefOk,secondRefOk,thirdRefOk;
};

class k9VobuList : public QPtrList <k9Vobu> {
public:
    k9Vobu *findVobu(uint32_t sector);
private:
    k9Vobu * findVobu(uint32_t sector, uint32_t start, uint32_t end);
protected:
    int compareItems ( QPtrCollection::Item item1, QPtrCollection::Item item2 ) ;
};

class k9Cell : public QObject {
    Q_OBJECT
public:
    k9Cell(QObject *parent = 0, const char *name = 0);
    ~k9Cell();
    k9Vobu * addVobu(uint32_t _sector);
    void addNewVobus(char *_buffer,uint32_t _len,uint32_t _position,int _vobNum,long _vobPos);
    static int isNavPack (uchar *ptr);
    static streamType_t identifyStream( uchar *buffer,int * packetType );
    static int getStreamID(int type);
    k9Vobu * findVobu(uint32_t _oldSector);
    uint32_t getnewSize();
    void addTitle(k9DVDTitle *_title);
    bool getforceFactor();
    float getFactor();
public:
    k9VobuList vobus;
    int vts;
    int pgc;
    int vob;
    uint32_t startSector;
    uint32_t lastSector;
    //TO REMOVE    uint32_t ;
    //TO REMOVE    uint32_t ;
    //TO REMOVE    uint32_t newSize;
    uchar angleBlock;
    //TO REMOVE    int nbVideoNew,nbVideoOld;
    int id;
    bool selected;
    bool copied;
    QValueList<int> audio;
    QValueList<int> subpicture;
    k9CellList *cellList;
private:
    int numVobu;
    void addRefStream(k9Vobu *_vobu,uchar *_buffer,uint32_t _position);
    QPtrList <k9DVDTitle> m_titles;
};



class k9CellList : public QPtrList <k9Cell> {
private:
    uint32_t m_position;
    k9Cell * findCell(uint32_t sector);
    k9Cell * findCell(uint32_t sector, uint32_t start, uint32_t end);

public:
    k9CellList() {
        m_position=0;
    };
    k9Cell *addCell(int _vts, int _pgc,int _vob);
    k9Vobu *findVobu(uint32_t sector);

    void setPosition(const uint32_t& _value) {
        m_position = _value;
    }

    uint32_t getPosition() const {
        return m_position;
    }

};


#endif
