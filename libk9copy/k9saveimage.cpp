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
#include <stdio.h>

#include <qimage.h>
#include <kstandarddirs.h>

void k9SaveImage::drawImage(QImage *_image) {
    m_cpt++;
    if (m_cpt ==20) {
     m_image=*_image;
     m_decoder->pause();
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
}
void k9SaveImage::stop() {
    m_stop=true;
    wait();
    m_decoder->stop();
}

void k9SaveImage::addData(uchar *_buffer, uint32_t _size) {
    if (m_timer.elapsed() >=7000 ) {
        if (m_mutex.tryLock()) {
            m_size=_size;

            m_buffer=new uchar[m_size];
            tc_memcpy(m_buffer,_buffer,m_size);
            m_mutex.unlock();
            start(LowestPriority);
        }
        m_timer.restart();
    }
}


void k9SaveImage::run() {
    m_mutex.lock();
    m_cpt=0;
    m_decoder->decode(m_buffer ,m_buffer+m_size,0);
    delete m_buffer;
    QString sFileName=m_tempFile->name();
    m_image.save(sFileName,"PNG");
    sFileName="\rINFOIMAGE:"+sFileName;
    fprintf(stderr,sFileName.latin1());

    m_mutex.unlock();
}

#include "k9saveimage.moc"
