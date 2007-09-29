//
// C++ Implementation: k9drawimage
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9drawimage.h"
#include <qpainter.h>

k9DrawImage::k9DrawImage(QWidget *parent, const char *name)
 : QWidget(parent, name)
{
}


k9DrawImage::~k9DrawImage()
{
}

void k9DrawImage::setImage(const QImage &_image){
    m_pixmap =_image;
    repaint();
}

void k9DrawImage::setImage(QString _fileName) {
        m_pixmap.load(_fileName);
        repaint();
}

void k9DrawImage::paintEvent ( QPaintEvent * ) {
        int top,left;
        QPainter p(this);
        
        double wratio=(double)width()/(double)m_pixmap.width();
        double hratio=(double)height()/(double)m_pixmap.height();
        double ratio= wratio < hratio ? wratio:hratio;

        top =(int) (height() -m_pixmap.height()*ratio)/2+1;
        left =(int) (width() -m_pixmap.width()*ratio)/2 ;

        p.scale(ratio,ratio);
        p.drawPixmap((int)(left/ratio),(int)(top/ratio),m_pixmap);
    }
#include "k9drawimage.moc"
