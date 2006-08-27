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

#include "k9common.h"

#ifdef HAVE_OPENGL

#include <qimage.h>
#include <qlibrary.h>
#include <qgl.h>
#include <qptrqueue.h>
#include <qmutex.h>

typedef void (*glClear_t) (GLbitfield);
typedef void (*glRasterPos2i_t) ( GLint , GLint );
typedef void (*glPixelZoom_t) ( GLfloat , GLfloat );
typedef void (*glDrawPixels_t) ( GLsizei , GLsizei, GLenum , GLenum ,const GLvoid * );
typedef void (* glFlush_t) ( void );
typedef void (*glClearColor_t)( GLclampf , GLclampf , GLclampf , GLclampf  );
typedef void (*glShadeModel_t) ( GLenum );
typedef void (*glPixelStorei_t)( GLenum , GLint  );
typedef void (*glViewport_t) ( GLint, GLint,GLsizei , GLsizei);
typedef void (*glMatrixMode_t)( GLenum );
typedef void (*glLoadIdentity_t) (void);
typedef void (*glOrtho_t)( GLdouble , GLdouble ,GLdouble , GLdouble, GLdouble , GLdouble);
typedef void (*glDisable_t) ( GLenum );
typedef void (*glPixelTransferi_t) ( GLenum , GLint );
typedef const GLubyte * (*glGetString_t)( GLenum );
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9GLWidget : public QGLWidget
{
Q_OBJECT
public:
    k9GLWidget(QWidget *parent = 0, const char *name = 0);
    void setImage(uchar *_buffer,int _width,int _height,int _len);
    ~k9GLWidget();
protected:
	void paintGL();
        void resizeGL(int width, int height);
        void showEvent(QShowEvent *event);
	void initializeGL();
private:
	QImage m_image;
	QMutex m_mutex;
	QPtrQueue<uchar> m_queue;
	int m_width,m_height;
	uchar *m_buffer;
	QLibrary * library;
	glClear_t glClear;
	glRasterPos2i_t glRasterPos2i;

	glPixelZoom_t glPixelZoom;
	glDrawPixels_t glDrawPixels;
	glFlush_t glFlush;
	glClearColor_t glClearColor;
	glShadeModel_t glShadeModel;
	glPixelStorei_t glPixelStorei;
	glViewport_t glViewport;
	glMatrixMode_t glMatrixMode;
	glLoadIdentity_t glLoadIdentity;
	glOrtho_t glOrtho;
        glDisable_t glDisable;
	glPixelTransferi_t glPixelTransferi;
	glGetString_t glGetString;
};

#else

#include <qwidget.h>
#include <qimage.h>

class k9GLWidget:public QWidget
{
public:
    k9GLWidget(QWidget *parent = 0, const char *name = 0){};
    void setImage(uchar *_buffer,int _width,int _height,int _len){};

};


#endif
#endif

