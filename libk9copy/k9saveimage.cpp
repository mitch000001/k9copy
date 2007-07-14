//
// C++ Implementation: k9saveimage
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9saveimage.h"
#include "kdecmpeg2.h"

#include <qimage.h>
#include <qfile.h>
#include <kstandarddirs.h>

void k9SaveImage::drawImage(QImage *_image) {
    m_cpt++;
    if (m_cpt >=20) {
     m_timer.restart();
     QFile m_stderr;   
     m_stderr.open(IO_WriteOnly,stderr);
     QString sFileName=m_tempFile->name();
    _image->save(sFileName,"PNG");
     sFileName="\rINFOIMAGE:"+sFileName;
     m_stderr.writeBlock(sFileName.latin1(),sFileName.length());
     m_stderr.close();
     m_cpt=0;
    }
}


k9SaveImage::k9SaveImage()
 : QObject(),QThread()
{
    m_tempFile= new KTempFile(locateLocal("tmp", "k9copy/k9p"), "");
    m_tempFile->setAutoDelete(true);
    m_decoder=new kDecMPEG2();
    connect(m_decoder, SIGNAL(pixmapReady(QImage *)), this, SLOT(drawImage(QImage *)));
}


k9SaveImage::~k9SaveImage()
{
    delete m_decoder;
    delete m_tempFile;
}

void k9SaveImage::play() {
    m_cpt=0;
    m_stop=false;
    m_timer.start();
    m_decoder->start();
    start(LowestPriority);
}
void k9SaveImage::stop() {
    m_stop=true;
    m_fifo.clear();
    wait();
    m_decoder->stop();
}

void k9SaveImage::addData(uchar *_buffer, uint32_t _size) {
    m_fifo.enqueue(_buffer,_size);
}

bool k9SaveImage::testFrameType(FrameType _type,uchar *_buffer) {
    //offset of frame
    uint start=0x16+_buffer[0x16]+1;
    //PES Length
    uint length=_buffer[0x12];
    length=(length <<8 ) + _buffer[0x13];

    //end of PES
    uint end=0x14+length;

    for (uint i=start;i <end-7;i++) {

        //looking for start code picture
        if ((_buffer[i-5] ==0) &&( _buffer[i-4] ==0)	&&( _buffer[i-3] ==1)&&( _buffer[i-2] ==0)) {
            uchar frameType=_buffer[i] &0x38;
            if (frameType==_type)
                return true;
        }
    }
    return false;
}

void k9SaveImage::run() {
    do {   
        uchar buffer[2048];
        m_fifo.dequeue(buffer,2048);
        if (m_timer.elapsed() >4000 ) {
            m_decoder->decode(buffer ,buffer+2048,0);
 
       } 
    } while (!m_stop);
}

#include "k9saveimage.moc"
