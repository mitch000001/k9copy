//
// C++ Implementation: k9menueditor
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9menueditor.h"
#include "k9menubutton.h"
#include "k9menuedit.h"
#include "k9canvasselection.h"
#include <qwmatrix.h>
#include <qcursor.h>
#include <kcursor.h>
k9MenuEditor::k9MenuEditor(
    QCanvas& c, QWidget* parent,
    const char* name, WFlags f) :
        QCanvasView(&c,parent,name,f) {
    this->setMaximumSize(QSize(724,580));
    moving=NULL;
    m_canvasSelection=new k9CanvasSelection(this);
    viewport()->setMouseTracking(true);
}


void k9MenuEditor::contentsMouseReleaseEvent(QMouseEvent* e) {
   m_canvasSelection->release();
}


void k9MenuEditor::contentsMousePressEvent(QMouseEvent* e) {
    QPoint p = inverseWorldMatrix().map(e->pos());
    moving_start = p;
    QCanvasItemList l=canvas()->collisions(p);
    moving=0;
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
       QCanvasItem *item=*it; 
       if (item->rtti()==QCanvasItem::Rtti_Rectangle)
        moving = item;
       if (item->rtti()>2000) {
            moving=item;
            return;
        }
    }
    if (moving)
        return;
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        moving = *it;
        if (moving->rtti()==1000) {
            if (e->state() & QMouseEvent::ControlButton)
                addSelection(moving);
            else {
                if (!isSelected(moving)) {
                    clearSelection();
                    addSelection(moving);
                }
            }
            m_canvasSelection->setPosition(p);
            m_canvasSelection->drawSelection();
        } else if (moving->rtti() <1001)
            clearSelection();
        emit itemSelected();
        return;
    }
    moving = 0;
    m_canvasSelection->setPosition(p);
    clearSelection();
    emit itemSelected();
}

void k9MenuEditor::clear() {
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        if ( *it )
            delete *it;
    }
}

void k9MenuEditor::clearSelection() {
    for (k9MenuButton *b=m_selection.first();b;b=m_selection.next())
        b->select(false);
    m_selection.clear();
}

bool k9MenuEditor::isSelected(QCanvasItem *_item) {
    k9CanvasSprite *s=(k9CanvasSprite *)_item;
    k9MenuButton *button=s->getButton();
    if (m_selection.find(button) !=-1)
        return true;
    else
        return false;
}

void k9MenuEditor::addSelection(QCanvasItem *_item) {
    k9CanvasSprite *s=(k9CanvasSprite *)_item;
    k9MenuButton *button=s->getButton();
    if (m_selection.find(button) !=-1) {
        button->select(false);
        m_selection.remove(button);
    } else {
        button->select(true);
        m_selection.append(button);
    }
}

void k9MenuEditor::updateCursor(QMouseEvent *e) {
   QPoint p = inverseWorldMatrix().map(e->pos());
   QCanvasItemList l=canvas()->collisions(p);
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
       QCanvasItem *item=*it; 
       switch (item->rtti()) {
            case QCanvasItem::Rtti_Rectangle :
                setCursor(KCursor::sizeAllCursor());
                return;
            case 2002 :
            case 2008:
                setCursor(KCursor::sizeFDiagCursor());
                return;
            case 2003:
            case 2007:
                setCursor(KCursor::sizeVerCursor());
                return;
            case 2004:
            case 2006:
                setCursor(KCursor::sizeBDiagCursor());
                return;
            case 2005:
            case 2009:
                setCursor(KCursor::sizeHorCursor());
                return;
            default:
                setCursor(KCursor::arrowCursor());
        }
    }
    setCursor(KCursor::arrowCursor());
}

void k9MenuEditor::contentsMouseMoveEvent(QMouseEvent* e) {
    updateCursor(e);
    if ( moving && (e->state() & Qt::LeftButton ==Qt::LeftButton) ) {
        if (moving->rtti() !=QCanvasItem::Rtti_Text || moving==m_menu->getText()) {
            QPoint p = inverseWorldMatrix().map(e->pos());
            int offsetX=p.x() - moving_start.x();
            int offsetY=p.y() - moving_start.y();
            moving_start = p;
            if (moving->rtti()==1000 || moving->rtti() == QCanvasItem::Rtti_Rectangle) {
                for (k9MenuButton *b=m_selection.first();b;b=m_selection.next()) {
                    k9CanvasSprite*spr=b->getSprite();
                    spr->moveBy(offsetX,offsetY);
                    spr->update();
                }

            } else
                moving->moveBy(offsetX,offsetY);
/*
            if (moving->rtti() >1001 && moving->rtti() <1010) {
                k9CanvasSpriteRedim *sprr=(k9CanvasSpriteRedim*)moving;
                sprr->update();
            }
*/  
            if (moving->rtti() >2001 && moving->rtti() <2010) {
                k9CanvasSelectionRedim *ssr=(k9CanvasSelectionRedim*)moving;
                ssr->updateSelection();
            } else {
                //if selection not resized, move the selection
                m_canvasSelection->moveBy(offsetX,offsetY);
                m_canvasSelection->update();
            }

            if (moving==m_menu->getText())
                emit m_menu->updateTextPos(QPoint(moving->x(),moving->y()));
            canvas()->update();
        }
    } else if ((e->state() & Qt::LeftButton) ==Qt::LeftButton ) {
        m_canvasSelection->hide();
        canvas()->update();
        QPoint p = inverseWorldMatrix().map(e->pos());
        int offsetX=p.x() - moving_start.x();
        int offsetY=p.y() - moving_start.y();
        m_canvasSelection->setSize(offsetX,offsetY);
        m_canvasSelection->show();
        canvas()->update();
        clearSelection();
        QCanvasItemList l=canvas()->collisions(m_canvasSelection->getRect());
        for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
            QCanvasItem *item = *it;
            if (item->rtti()==1000) {
                addSelection(item);
            }
        }
    }
}

void k9MenuEditor::resizeEvent ( QResizeEvent * e ) {
    QWMatrix m;
    double scalex=(e->size().width()-4.0)/720.0;
    double scaley=(e->size().height()-4.0)/576.0;
    m.scale(QMIN(scalex,scaley),QMIN(scalex,scaley));
    this->setWorldMatrix(m);

}

k9MenuButton * k9MenuEditor::getSelected()  {
    if (moving) {
        if (moving->rtti()==1000) {
            k9CanvasSprite *s=(k9CanvasSprite *)moving;
            return s->getButton();
        }
    }
    return NULL;
}

QCanvasItem* k9MenuEditor::getMoving() const {
    return moving;
}


void k9MenuEditor::setMoving(QCanvasItem* _value) {
    moving = _value;
}



QPtrList< k9MenuButton > *k9MenuEditor::getSelection()  {
    return &m_selection;
}

void k9MenuEditor::hideSelection() {
    m_canvasSelection->hide();
}

#include "k9menueditor.moc"
