/**************************************************************************
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
#include "k9common.h"
#include <qpixmap.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpaintdevice.h>
#include <qslider.h>
#include <qapplication.h>
#include <qtoolbutton.h>
#include <qlayout.h>

#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <klocale.h>
#include <kiconloader.h>
#include "kviewmpeg2.h"
#include "k9config.h"



k9Widget::k9Widget(QWidget *parent):QWidget(parent) {
    m_image=NULL;
}

void k9Widget::setImage(QImage *_image) {
    m_image=_image;
    //paintEvent( NULL);
    repaint(FALSE);
}

void k9Widget::paintEvent( QPaintEvent *_event) {
    setPaletteBackgroundColor(Qt::black);
    int top,left;
    if (m_image !=NULL) {
        QPainter p(this);
        double wratio=(double)width()/(double)m_image->width();
        double hratio=(double)height()/(double)m_image->height();
        double ratio= wratio < hratio ? wratio:hratio;

        top =(int) (height() -m_image->height()*ratio)/2+1;
        left =(int) (width() -m_image->width()*ratio)/2 +1;

        p.scale(ratio,ratio);

        p.drawImage((int)(left/ratio),(int)(top/ratio),*m_image);

        p.end();
    }
}

kViewMPEG2::kViewMPEG2() {
    m_widget=NULL;
    m_GLwidget=NULL;
    m_layout=NULL;
    bPlay->setPixmap(SmallIcon("player_play"));
    bStop->setPixmap(SmallIcon("player_stop"));
    connect(&m_player  , SIGNAL(setPosition(uint32_t)), this, SLOT(setPosition(uint32_t)));
    connect(&m_player  , SIGNAL(setMax(uint32_t)), this, SLOT(setMax(uint32_t)));
    connect(&m_player  , SIGNAL(setMin(uint32_t)), this, SLOT(setMin(uint32_t)));
    lockSlider=false;
    stopped=true;

    m_layout=new QGridLayout(label,1,1);

#ifdef HAVE_OPENGL

    readSettings();
#else

    m_prefUseGL=FALSE;
#endif
    //disable the option use_gl, so if k9copy crash, we restart without gl
    if (m_prefUseGL) {
    	k9Config config;
    	config.setUseGL( false);
    	config.save();
    }
    m_player.getDecoder()->setUseGL(m_prefUseGL);
    if (m_prefUseGL)  {
        connect(m_player.getDecoder()  , SIGNAL(ppmReady(uchar *,int,int,int)), this, SLOT(drawppm(uchar *,int,int,int)));
        
        m_GLwidget= k9GLWidget::createWidget(label);
        m_widget=NULL;
        m_layout->addWidget(m_GLwidget,0,0);
        
        //init ok, we can put the gl flag to true
    	k9Config config;
    	config.setUseGL( true);
    	config.save();
        
    } else {
        connect(m_player.getDecoder()  , SIGNAL(pixmapReady(QImage *)), this, SLOT(drawPixmap(QImage *)));
        m_widget=new k9Widget(label);
        m_layout->addWidget(m_widget,0,0);

        m_GLwidget=NULL;
    }

}


void kViewMPEG2::readSettings() {
    m_player.stop();

    //KSimpleConfig settings("K9Copy");
    k9Config config;
    m_prefUseGL=config.getUseGL();
    //m_prefUseGL=settings.readEntry("/options/useGL",0).toInt();

}


kViewMPEG2::~kViewMPEG2() {
    stopped=true;
    m_player.stop();
}


void kViewMPEG2::lock() {
    mutex.lock();
}

void kViewMPEG2::unlock() {
    mutex.unlock();
}

void kViewMPEG2::setPosition( uint32_t _position) {
    if (!lockSlider)
        slider->setValue(_position);
}

void kViewMPEG2::setMax( uint32_t _position) {
    slider->setMaxValue(_position);
}

void kViewMPEG2::setMin(uint32_t _position) {
    slider->setMinValue(_position);
}


/** No descriptions */
void kViewMPEG2::drawPixmap(QImage *image) {
    if (qApp==NULL)
        return;
    if (qApp->tryLock()) {
        m_widget->setImage( image);
        if (qApp!=NULL)
            qApp->unlock();
        }
}

/** No descriptions */
void kViewMPEG2::drawppm(uchar *_buf,int _width,int _height,int _len) {
    if (qApp==NULL)
        return;
//    if (qApp->tryLock()) {
        m_GLwidget->setImage(_buf,_width,_height,_len);
 //       if (qApp !=NULL)
  //          qApp->unlock();
 //   } else
 //       free(_buf);
}


void kViewMPEG2::bPlayClick() {
    m_player.play();
}


int kViewMPEG2::open (k9DVD *_dvd,k9DVDTitle * title,int chapter) {
    m_player.open(_dvd->getdvd()->getDvd(), _dvd->getDevice(),title,chapter);
}


/** No descriptions */
void kViewMPEG2::setError(const QString & err) {
    error=true;
    errMsg=err;
}


void kViewMPEG2::resizeEvent(QResizeEvent *_event) {}

void kViewMPEG2::bStopClick() {
    m_player.stop();
}

void kViewMPEG2::sliderReleased() {
    pause=true;
    int i;
    i=slider->value();
    m_player.updatePos(i);
    lockSlider=false;

}

void kViewMPEG2::closeEvent( QCloseEvent* ce ) {
    stopped=true;
    ce->accept();
    return;

}
/** No descriptions */
void kViewMPEG2::sliderPressed() {
    lockSlider=true;
}
