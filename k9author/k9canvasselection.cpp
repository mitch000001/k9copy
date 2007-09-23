// //
// C++ Implementation: k9canvasselection
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9canvasselection.h"
#include "k9menubutton.h"

k9CanvasSelectionRedim::k9CanvasSelectionRedim (ePosition _position,int _x,int _y,int _w,int _h,QCanvas *canvas,k9CanvasSelection *selection):QCanvasRectangle(_x,_y,_w,_h,canvas) {
    m_position=_position;
    m_selection=selection;
}

void k9CanvasSelectionRedim::moveBy(double _x,double _y) {
    m_offsetX=_x;
    m_offsetY=_y;
    QCanvasRectangle::moveBy(_x,_y);
}

void k9CanvasSelectionRedim::updateSelection() {
    int w,h;
    int w1=m_selection->getWidth();
    int h1=m_selection->getHeight();
    int x1=m_selection->getLeft();
    int y1=m_selection->getTop();
    switch (m_position) {
    case RedimTopLeft:
        w=m_selection->getWidth()-m_offsetX;
        h=m_selection->getHeight()-m_offsetY;
        if (m_selection->canScale(-m_offsetX,-m_offsetY)) {
            m_selection->setWidth(w);
            m_selection->setHeight(h);
            m_selection->moveBy(m_offsetX,m_offsetY);
        }
        break;
    case RedimMiddleRight :
        w= m_selection->getWidth()+ m_offsetX;
        if (m_selection->canScale(m_offsetX,0)) 
            m_selection->setWidth(w);
        break;
    case RedimTopMiddle:
        h=m_selection->getHeight()-m_offsetY;
        if (m_selection->canScale(0,-m_offsetY)) {
            m_selection->setHeight(h);
            m_selection->moveBy(0,m_offsetY);
        }
        break;
    case RedimTopRight:
        h=m_selection->getHeight()-m_offsetY;
        w=m_selection->getWidth()+ m_offsetX;
        if (m_selection->canScale(m_offsetX,-m_offsetY)) {
            m_selection->setHeight(h);
            m_selection->setWidth( w);
            m_selection->moveBy(0,m_offsetY);
        }
        break;
    case RedimMiddleLeft:
        w=m_selection->getWidth()-m_offsetX;
        if (m_selection->canScale(-m_offsetX,0)) {
            m_selection->setWidth(w);
            m_selection->moveBy(m_offsetX,0);
        }
        break;
    case RedimBottomLeft:
        w=m_selection->getWidth()-m_offsetX;
        h=m_selection->getHeight()+m_offsetY;
       if (m_selection->canScale(-m_offsetX,+m_offsetY)) {
            m_selection->setWidth(w);
            m_selection->setHeight(h);
            m_selection->moveBy(m_offsetX,0);
        }
        break;
    case RedimBottomMiddle:
        h=m_selection->getHeight()+m_offsetY;
        if (m_selection->canScale(0,+m_offsetY)) 
            m_selection->setHeight(h);
        break;

    case RedimBottomRight:
        w=m_selection->getWidth()+m_offsetX;
        h=m_selection->getHeight()+m_offsetY;
        if (m_selection->canScale(+m_offsetX,+m_offsetY)) {
            m_selection->setWidth(w);
            m_selection->setHeight(h);
        }
        break;
    }
    m_selection->scale(m_selection->getLeft()-x1,m_selection->getTop()-y1, m_selection->getWidth()-w1,m_selection->getHeight()-h1);
    m_selection->update();

}


k9CanvasSelection::k9CanvasSelection(k9MenuEditor *_editor)
        : QObject(_editor) {
    m_rect=NULL;
    m_editor=_editor;
}


k9CanvasSelection::~k9CanvasSelection() {}

void k9CanvasSelection::setPosition(QPoint _p) {
    if (m_rect==NULL) {
        QCanvas *m_canvas=m_editor->canvas();
        m_rect=new QCanvasRectangle(m_editor->canvas());
        QBrush brush(Qt::green);
        m_rm=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimMiddleRight,0,0,10,10,m_canvas,this);
        m_rm->setPen(Qt::black);
        m_rm->setBrush(brush);

        m_lt=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimTopLeft,0,0,10,10,m_canvas,this);
        m_lt->setPen(Qt::black);
        m_lt->setBrush(brush);

        m_mt=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimTopMiddle,0,0,10,10,m_canvas,this);
        m_mt->setPen(Qt::black);
        m_mt->setBrush(brush);

        m_mt=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimTopMiddle,0,0,10,10,m_canvas,this);
        m_mt->setPen(Qt::black);
        m_mt->setBrush(brush);

        m_rt=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimTopRight,0,0,10,10,m_canvas,this);
        m_rt->setPen(Qt::black);
        m_rt->setBrush(brush);

        m_lm=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimMiddleLeft,0,0,10,10,m_canvas,this);
        m_lm->setPen(Qt::black);
        m_lm->setBrush(brush);

        m_lb=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimBottomLeft,0,0,10,10,m_canvas,this);
        m_lb->setPen(Qt::black);
        m_lb->setBrush(brush);

        m_mb=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimBottomMiddle,0,0,10,10,m_canvas,this);
        m_mb->setPen(Qt::black);
        m_mb->setBrush(brush);

        m_rb=new k9CanvasSelectionRedim(k9CanvasSelectionRedim::RedimBottomRight,0,0,10,10,m_canvas,this);
        m_rb->setPen(Qt::black);
        m_rb->setBrush(brush);
    } else {
        m_rect->setCanvas(m_editor->canvas());
        m_lt->setCanvas(m_editor->canvas());
        m_mt->setCanvas(m_editor->canvas());
        m_rt->setCanvas(m_editor->canvas());
        m_lm->setCanvas(m_editor->canvas());
        m_rm->setCanvas(m_editor->canvas());
        m_lb->setCanvas(m_editor->canvas());
        m_mb->setCanvas(m_editor->canvas());
        m_rb->setCanvas(m_editor->canvas());
        ;
    }
    m_rect->setPen(QPen(Qt::red));
    m_rect->setX(_p.x());
    m_rect->setY(_p.y());
    m_rect->setSize(0,0);

}

void k9CanvasSelection::release() {
    if (m_rect) {
        if (m_editor->getSelection()->count() !=0) {
            drawSelection();
        } else {
            hide();
            hideRedim();
        }
        m_editor->canvas()->update();
    }
}

void k9CanvasSelection::drawSelection() {
    int left=720,top=576,right=0,bottom=0;
    QPtrList <k9MenuButton> *selection=m_editor->getSelection();

    for (k9MenuButton *b=selection->first();b;b=selection->next()) {
        left=QMIN(left,b->getLeft());
        top=QMIN(top,b->getTop());
        right=QMAX(right,b->getWidth()+b->getLeft());
        bottom=QMAX(bottom,b->getTop()+b->getHeight());
    }
    m_rect->setX(left-2);
    m_rect->setY(top-2);
    m_rect->setSize(right-left+4,bottom-top+4);
    update();
}

void k9CanvasSelection::update() {
    int x=(int)m_rect->x() -5;
    int y=(int)m_rect->y()-5;
    int w=getWidth();
    int h=getHeight();

    int count=m_editor->canvas()->allItems().count();

    m_lt->move(x,y);
    m_lt->setZ(count--);

    m_mt->move(x+w/2,y);
    m_mt->setZ(count--);

    m_rt->move(x+w,y);
    m_rt->setZ(count--);

    m_lm->move(x,y+h/2);
    m_lm->setZ(count--);

    m_rm->move(x+w,y+h/2);
    m_rm->setZ(count--);

    m_lb->move(x,y+h);
    m_lb->setZ(count--);

    m_mb->move(x+w/2,y+h);
    m_mb->setZ(count--);

    m_rb->move(x+w,y+h);
    m_rb->setZ(count--);

    m_rect->setZ(count--);

    QCanvasItemList l=m_editor->canvas()->allItems();
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
       QCanvasItem *item=*it;
       if (item != m_rect && item !=m_lt && item !=m_rt && item != m_mt && item !=m_lm && item != m_rm && item != m_lb && item != m_mb && item !=m_rb)  
            item->setZ(count--);
    }

    showRedim();
    show();
}

void k9CanvasSelection::scale(int _x,int _y,int _w, int _h) {
    QPtrList <k9MenuButton> *buttons=m_editor->getSelection();
    for (k9MenuButton *b=buttons->first();b;b=buttons->next()) {
        b->moveBy(_x,_y,false);
        if (b->getWidth() + _w >10)
            b->setWidth(b->getWidth()+_w,false);
        if (b->getHeight() +_h >10)
            b->setHeight(b->getHeight()+_h);
    }

}

bool k9CanvasSelection::canScale(int _w, int _h) {
    QPtrList <k9MenuButton> *buttons=m_editor->getSelection();
    for (k9MenuButton *b=buttons->first();b;b=buttons->next()) {
        if (b->getWidth() + _w <10)
            return false;
        if (b->getHeight() +_h <10)
            return false;
    }
    return true;
}


void k9CanvasSelection::hide() {
    m_rect->hide();
    hideRedim();
}
void k9CanvasSelection::hideRedim() {
    m_lt->hide();
    m_mt->hide();
    m_rt->hide();
    m_lm->hide();
    m_rm->hide();
    m_lb->hide();
    m_mb->hide();
    m_rb->hide();

}

void k9CanvasSelection::show() {
    m_rect->show();
}
void k9CanvasSelection::showRedim() {
    m_lt->show();
    m_mt->show();
    m_rt->show();
    m_lm->show();
    m_rm->show();
    m_lb->show();
    m_mb->show();
    m_rb->show();

}


void k9CanvasSelection::setSize(int _w,int _h) {
    m_rect->setSize(_w,_h);
}

QRect k9CanvasSelection::getRect() {
    return m_rect->rect();

}

void k9CanvasSelection::moveBy(int _x,int _y) {
    m_rect->moveBy(_x,_y);
}

int k9CanvasSelection::getWidth() {
    return m_rect->width();
}
void k9CanvasSelection::setWidth(int _w) {
    m_rect->setSize(_w,m_rect->height());
}
int k9CanvasSelection::getHeight() {
    return m_rect->height();
}
void k9CanvasSelection::setHeight(int _h) {
    m_rect->setSize(m_rect->width(),_h);
}

int k9CanvasSelection::getLeft() {
    return (int)m_rect->x();
}
int k9CanvasSelection::getTop() {
    return (int)m_rect->y();
}

#include "k9canvasselection.moc"
