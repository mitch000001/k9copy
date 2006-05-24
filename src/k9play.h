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
class k9play{
private:
    int m_title;
    k9DVD *m_dvd;
    k9DVDTitle *m_dvdTitle;
    QString m_device;
    void printPosition();
    QFile m_output;
    QFile m_stderr;
    uint32_t m_totalBytes;
    uint32_t m_startSector,m_endSector;
    QStringList m_audioFilter;
    QStringList m_subpictureFilter;
    void kdebug(QString const & _msg);
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
};

#endif
