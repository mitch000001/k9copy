//
// C++ Implementation: k9glwidget
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9glwidget.h"
#include <qpainter.h>
#include <qsize.h>
k9GLWidget::k9GLWidget(QWidget *parent, const char *name)
        : QGLWidget(parent, name) {
}


k9GLWidget::~k9GLWidget() {}

void k9GLWidget::setImage(QImage _image) {
   m_image=_image;
   glDraw();

}

void k9GLWidget::paintGL() {

    GLfloat wratio=(GLfloat)width()/(GLfloat)m_image.width();
    GLfloat hratio=(GLfloat)height()/(GLfloat)m_image.height();
    GLfloat ratio= wratio < hratio ? wratio:hratio;	
    QImage icv = QGLWidget::convertToGLFormat(m_image );

   glClear(GL_COLOR_BUFFER_BIT);
   int top = (int) (height() -m_image.height()*ratio) /2;
   int left = (int) (width() -m_image.width()*ratio) /2;
   glRasterPos2i (left, top);
   glPixelZoom (ratio, ratio);
    glDrawPixels( m_image.width(), m_image.height(), GL_RGBA, GL_UNSIGNED_BYTE, icv.bits() );
   glFlush ();
}


void k9GLWidget::initializeGL() {
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void k9GLWidget::resizeGL(int w, int h) {
 glViewport (0, 0, w, h);
 glMatrixMode (GL_PROJECTION);     
 glLoadIdentity();
 glOrtho(0, w,0,h,-1,1);
 glMatrixMode (GL_MODELVIEW);
}


void k9GLWidget::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
}

#include "k9glwidget.moc"
