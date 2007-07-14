//
// C++ Interface: k9saveimage
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9SAVEIMAGE_H
#define K9SAVEIMAGE_H

#include "k9common.h"
#include <qthread.h>
#include <qobject.h>
#include <qdatetime.h>

#include "k9fifo.h"
#include <ktempfile.h>


/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
typedef enum  { FrameType_B=0x18,FrameType_I=0x08,FrameType_P=0x10,FrameType_D=0x20} FrameType;

class kDecMPEG2;

class k9SaveImage : public QObject,QThread
{
Q_OBJECT
public:
    k9SaveImage();
    ~k9SaveImage();

    void play();
    void stop();
    void addData(uchar *_buffer, uint32_t _size);
protected:
    void run();
    bool testFrameType(FrameType _type,uchar *_buffer);
public slots:
   void drawImage(QImage*);

private:
     kDecMPEG2 *m_decoder;
     k9fifo m_fifo;
     bool m_stop;
     QTime m_timer;
     uint m_cpt;
     KTempFile *m_tempFile;
};

#endif
