//
// C++ Interface: k9play
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9PLAY_H
#define K9PLAY_H

#include "k9common.h"
#include "k9dvdtitle.h"
#include <qfile.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

typedef struct {
 uint title;
 uint chapter;
 uint cell;
 uint32_t sector;
 uint64_t bytesWritten;
 uint64_t bytesRead;
 uint64_t bytesSkipped;
 uint64_t bytesChapters;
} k9play_st;

class k9play{
private:
    int m_title;
    k9DVD *m_dvd;
    k9DVDTitle *m_dvdTitle;
    QString m_device;
    QFile m_output;
    QFile m_stderr;
    uint32_t m_totalBytes;
    uint32_t m_startSector,m_endSector;
    double m_vampsFactor;
    uint64_t m_inputSize;
    uint64_t m_totalSize;
    uint64_t m_chapterSize;
    uint64_t m_dvdSize;
    uint m_chapter;
    uint m_cell;
    bool m_initstatus;
    bool m_continue;
    QString m_inject;
    QStringList m_audioFilter;
    QStringList m_subpictureFilter;
    void kdebug(QString const & _msg);
    bool readNavPack (k9DVDFile *fh, dsi_t *dsi,int sector, uchar *_buffer);
    void insert_dummy_pack (int8_t *buf);
    void insert_nav_pack (int8_t *buf);
    void saveStatus(k9play_st _status);
    void readStatus(k9play_st &_status);
public:
    k9play();

    ~k9play();
    void execute();
    void play();
    void setTitle(int _value) {m_title = _value;};

    void setDevice(const QString& _value) {m_device = _value;};
	
    void setstartSector(QString _value);
    void setendSector(QString _value);
    void setaudioFilter(QString _value);
    void setsubpictureFilter(QString _value);
    void setvampsFactor(QString _value);
    void setinputSize(QString _value);
    void settotalSize(QString _value);
    void setchapter(QString _value);
    void setcell(QString _value);
    void setinject(QString _value);
    void setdvdSize(QString _value);
    void setchapterSize(QString _value);
    void setinitStatus(bool _value);
    void setcontinue (bool _value);
};

#endif
