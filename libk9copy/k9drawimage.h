//
// C++ Interface: k9drawimage
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DRAWIMAGE_H
#define K9DRAWIMAGE_H

#include <qwidget.h>
#include <qpixmap.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9DrawImage : public QWidget
{
Q_OBJECT
public:
    k9DrawImage(QWidget *parent = 0, const char *name = 0);
    void setImage(QString _fileName);
    void setImage(const QImage &_image);
    ~k9DrawImage();
private:
    QPixmap m_pixmap;

protected:
    void paintEvent ( QPaintEvent *);
};

#endif
