//
// C++ Interface: k9decodethread
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DECODETHREAD_H
#define K9DECODETHREAD_H
#include "k9common.h"
#include "k9vamps.h"

#include <qthread.h>
#include <qobject.h>
#include <qwaitcondition.h>

#include "kdecmpeg2.h"

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9DecodeThread : public QThread
{
private:
    kDecMPEG2 *m_decoder;
    k9fifo m_fifo;
    QWaitCondition wDataRead;
    QWaitCondition wDataReady;
    bool noData;

public:
    k9DecodeThread();
    ~k9DecodeThread();
    void addData(uchar *data,uint size);
    void setNoData();
    void clear();
    kDecMPEG2 *getDecoder() {return m_decoder;}
	
protected:
    int readData(uchar * data,uint size);
    void run();



};

#endif
