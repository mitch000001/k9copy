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
#include "kviewmpeg2.h"
#include <qpixmap.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpaintdevice.h>
#include <qslider.h>
#include <qapplication.h>
#include <qtoolbutton.h>

#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <klocale.h>
#include <kiconloader.h>

kViewMPEG2::kViewMPEG2() {
    decoder=new kDecMPEG2;
    /*  label->setAutoResize(false);
      label->setScaledContents(false);
      label->setAlignment(Qt::AlignCenter);
      */

    bPlay->setPixmap(SmallIcon("player_play"));
    bStop->setPixmap(SmallIcon("player_stop"));
    connect(decoder, SIGNAL(pixmapReady(const QImage &)), this, SLOT(drawPixmap(const QImage &)));
    lockSlider=false;
    stopped=true;
}
kViewMPEG2::~kViewMPEG2() {
    stopped=true;
    delete decoder;
}
/** No descriptions */
void kViewMPEG2::drawPixmap(const QImage &image) {
    int top,left;
    if (!pause) {
        /*QPixmap pix(img);
        label->setPixmap(pix);
        qApp->processEvents();

        */
        QPainter p(label);
        //     img=image.scale(label->contentsRect().width(),label->contentsRect().height(),QImage::ScaleMin);
        img=image.scale(label->width(),label->height(),QImage::ScaleMin);
        top =(int) (label->height() -img.height())/2 +1;
        left =(int) (label->width() -img.width())/2 +1;
        p.drawImage(left,top,img);
        p.end();

    } else
        pause=false;
}

void kViewMPEG2::bPlayClick() {

    open(dev,m_title);
}


int kViewMPEG2::open (const QString & device,k9DVDTitle * title) {
    dvd_reader_t *dvd;
    int  ret = 0;
    struct stat dvd_stat;
    QString c;
    m_title=title;

    error=false;
    errMsg="";
    startSector=0;
    lastSector=0;

    stopped=false;

    dev=device;
    ret = stat(device.latin1(), &dvd_stat);
    if ( ret < 0 ) {
        c=i18n("Can't find device %1\n").arg(device.latin1());
        setError(c);
        return 1;
    }

    dvd = DVDOpen(device.latin1());
    if( !dvd ) {
        c=i18n("Can't open disc %1!\n").arg(device.latin1());
        setError(c);
        return 2;
    }

    startSector=title->getChapter(0)->getstartSector();
    lastSector=title->getChapter(title->getchapterCount()-1)->getendSector();
    slider->setMinValue(startSector);
    slider->setMaxValue(lastSector);
    slider->setValue(startSector);

    playDVD(dvd,title->getVTS());

    DVDClose(dvd);
    return 0;
}

void kViewMPEG2::playDVD(dvd_reader_t *dvd,int titleSetNr) {
    unsigned char buffer[DVD_VIDEO_LB_LEN*2];
    unsigned char *ptrbuff;

    int blocksize=2;
    ssize_t size;
    QString c;
    dvd_file_t *dvdfile;
    dvdfile = DVDOpenFile(dvd, titleSetNr, DVD_READ_TITLE_VOBS);

    if( !dvdfile ) {
        c=i18n("Error opening vobs for title %1\n").arg(titleSetNr);
        setError(c);
        return ;
    }

    idxLect = slider->value();
    idxLect=startSector;
    while( (idxLect <= lastSector) && (blocksize>0)) {
        if (idxLect+blocksize> lastSector)
            blocksize=lastSector-idxLect;
        if (blocksize<=0)
            break;
        size= DVDReadBlocks(dvdfile, idxLect,blocksize , buffer);
        if( !size ) {
            c=i18n("ERROR reading block %1\n").arg(idxLect);
            setError(c);
            break;
        }

        ptrbuff=buffer;
        decoder->decode(buffer,buffer+DVD_VIDEO_LB_LEN*blocksize,0);
        qApp->processEvents();
        idxLect+=blocksize;
        if (!lockSlider)
            slider->setValue(idxLect);

        if (stopped)
            break;
    }

    slider->setValue(lastSector);
    DVDCloseFile(dvdfile);
    /*   QPixmap pix(img);
       label->setPixmap(pix);*/
    stopped=true;
}


/** No descriptions */
void kViewMPEG2::setError(const QString & err) {
    error=true;
    errMsg=err;
}

void kViewMPEG2::bStopClick() {
    stopped=true;
    close();
    /*QPixmap pix(img);
    label->setPixmap(pix);
    */

}

void kViewMPEG2::sliderReleased() {
    pause=true;
    int i;
    i=slider->value();
    idxLect=i;
    decoder->restart();
    lockSlider=false;

}

void kViewMPEG2::closeEvent( QCloseEvent* ce ) {
    stopped=true;
    ce->accept();
    return;

    /*
        switch( QMessageBox::information( this, "Qt Application Example",
                                          "The document has been changed since "
                                          "the last save.",
                                          "Save Now", "Cancel", "Leave Anyway",
                                          0, 1 ) ) {
        case 0:
            save();
            ce->accept();
            break;
        case 1:
        default: // just for sanity
            ce->ignore();
            break;
        case 2:
            ce->accept();
            break;
        }     */
}
/** No descriptions */
void kViewMPEG2::sliderPressed() {
    lockSlider=true;
}
