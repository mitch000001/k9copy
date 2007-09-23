//
// C++ Interface: k9canvasselection
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9CANVASSELECTION_H
#define K9CANVASSELECTION_H

#include "k9menueditor.h"
#include <qcanvas.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9CanvasSelection;
class k9CanvasSelectionRedim : public QCanvasRectangle {
public:
    enum ePosition  { RedimTopLeft=2002,RedimTopMiddle=2003,RedimTopRight=2004,RedimMiddleLeft=2005,RedimBottomLeft=2006,RedimBottomMiddle=2007,RedimBottomRight=2008,RedimMiddleRight=2009} ;
    k9CanvasSelectionRedim (ePosition _position,int _x,int _y,int _w,int _h,QCanvas *canvas,k9CanvasSelection *selection);
    void updateSelection();
    virtual int rtti() const {
        return m_position;
    }
    virtual void moveBy(double _x,double _y);
private:
    k9CanvasSelection *m_selection;
    ePosition m_position;
    double m_offsetX,m_offsetY;
};


class k9CanvasSelection : public QObject
{
Q_OBJECT
public:
    k9CanvasSelection(k9MenuEditor *_editor);

    ~k9CanvasSelection();
    void setPosition (QPoint _p);
    void release();
    void hide();
    void hideRedim();
    void showRedim();
    void show();
    void setSize(int _w,int _h);
    QRect getRect();
    void moveBy(int _x,int _y);
    int getWidth();
    void setWidth(int _w);
    int getHeight();
    void setHeight(int _h);
    int getLeft();
    int getTop();
    void update();
    void scale(int _x,int _y, int _w,int _h);
    void drawSelection();
    bool canScale(int _w, int _h);

private:
    k9MenuEditor *m_editor;
    QCanvasRectangle *m_rect;
    k9CanvasSelectionRedim *m_lt,*m_mt,*m_rt,*m_lm,*m_rm,*m_lb,*m_mb,*m_rb;
};

#endif
