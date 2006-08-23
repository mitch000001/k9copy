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

#ifdef HAVE_OPENGL
#include <qsize.h>
#include <qapplication.h>
k9GLWidget::k9GLWidget(QWidget *parent, const char *name)
        : QGLWidget(parent, name) {

    library=new QLibrary("GL");

    glClear = (glClear_t) library->resolve( "glClear" );
    glRasterPos2i= (glRasterPos2i_t)library->resolve("glRasterPos2i");
    glFlush = (glFlush_t)library->resolve("glFlush");
    glClearColor =(glClearColor_t)library->resolve("glClearColor");
    glShadeModel =(glShadeModel_t)library->resolve("glShadeModel");
    glPixelStorei =(glPixelStorei_t)library->resolve("glPixelStorei");
    glViewport =(glViewport_t)library->resolve("glViewport");
    glMatrixMode =(glMatrixMode_t)library->resolve("glMatrixMode");
    glLoadIdentity =(glLoadIdentity_t)library->resolve("glLoadIdentity");
    glOrtho =(glOrtho_t)library->resolve("glOrtho");
    glPixelZoom=(glPixelZoom_t)library->resolve("glPixelZoom");
    glDrawPixels=(glDrawPixels_t)library->resolve("glDrawPixels");
    glDisable=(glDisable_t)library->resolve("glDisable");
    glPixelTransferi=(glPixelTransferi_t)library->resolve("glPixelTransferi");
    glGetString=(glGetString_t)library->resolve("glGetString");
}


k9GLWidget::~k9GLWidget() {

    delete library;
}

void k9GLWidget::setImage(QImage _image) {
    m_image=QGLWidget::convertToGLFormat(_image );
    update();
    qApp->wakeUpGuiThread();
}

void k9GLWidget::paintGL() {

    GLfloat wratio=(GLfloat)width()/(GLfloat)m_image.width();
    GLfloat hratio=(GLfloat)height()/(GLfloat)m_image.height();
    GLfloat ratio= wratio < hratio ? wratio:hratio;

    //    glClear(GL_COLOR_BUFFER_BIT);
    int top = (int) (height() -m_image.height()*ratio) /2;
    int left = (int) (width() -m_image.width()*ratio) /2;
    glRasterPos2i (left, top);

    glPixelZoom (ratio, ratio);

    glDrawPixels( m_image.width(), m_image.height(), GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits() );
    glFlush ();
}


void k9GLWidget::initializeGL() {
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_DITHER);
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_LOGIC_OP);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);

    glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
    glPixelTransferi(GL_RED_SCALE, 1);
    glPixelTransferi(GL_RED_BIAS, 0);
    glPixelTransferi(GL_GREEN_SCALE, 1);
    glPixelTransferi(GL_GREEN_BIAS, 0);
    glPixelTransferi(GL_BLUE_SCALE, 1);
    glPixelTransferi(GL_BLUE_BIAS, 0);
    glPixelTransferi(GL_ALPHA_SCALE, 1);
    glPixelTransferi(GL_ALPHA_BIAS, 0);

    /*
     * Disable extensions that could slow down glDrawPixels.
     */
    const GLubyte* extString = glGetString(GL_EXTENSIONS);

    if (extString != NULL) {
       if (strstr((char*) extString, "GL_EXT_convolution") != NULL) {
           glDisable(GL_CONVOLUTION_1D_EXT);
           glDisable(GL_CONVOLUTION_2D_EXT);
           glDisable(GL_SEPARABLE_2D_EXT);
       }
       if (strstr((char*) extString, "GL_EXT_histogram") != NULL) {
           glDisable(GL_HISTOGRAM_EXT);
           glDisable(GL_MINMAX_EXT);
       }
       if (strstr((char*) extString, "GL_EXT_texture3D") != NULL) {
           glDisable(GL_TEXTURE_3D_EXT);
       }
    }


}

void k9GLWidget::resizeGL(int w, int h) {
    glViewport (0, 0, w, h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w,0,h,-1,1);
    glMatrixMode (GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
}


void k9GLWidget::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
}

#include "k9glwidget.moc"

#endif
