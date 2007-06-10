//
// C++ Interface: k9audiocodecs
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9AUDIOCODECS_H
#define K9AUDIOCODECS_H

#include "k9common.h"
#include "k9config.h"
#include <qobject.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class _k9AudioCodec;
class k9AudioCodecs : public QObject
{
Q_OBJECT
public:
    k9AudioCodecs(QObject *parent = 0, const char *name = 0);

    ~k9AudioCodecs();
    void save();
    int count();
    void setOptions(int _num,QString _value);
    void setCodecName(int _num,QString _value);
    QString getOptions(int _num);
    QString getCodecName(int _num);
    void remove(int _num);
private:
    QMap <int,_k9AudioCodec> m_codecs;
    k9Config *m_config;

};

#endif
