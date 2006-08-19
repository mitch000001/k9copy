//
// C++ Interface: k9glwidget
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9GLWIDGET_H
#define K9GLWIDGET_H

#include <qgl.h>
#include <qimage.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9GLWidget : public QGLWidget
{
Q_OBJECT
public:
    k9GLWidget(QWidget *parent = 0, const char *name = 0);
    void setImage(QImage _image);
    ~k9GLWidget();
protected:
	void paintGL();
        void resizeGL(int width, int height);
        void showEvent(QShowEvent *event);
	void initializeGL();
private:
	QImage m_image;
};

#endif
