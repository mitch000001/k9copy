//
// C++ Interface: k9menueditor
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9MENUEDITOR_H
#define K9MENUEDITOR_H

#include <qcanvas.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9MenuButton;
class k9MenuEdit;
class k9CanvasSelection;
class k9MenuEditor : public QCanvasView {
    Q_OBJECT

public:
    k9MenuEditor(QCanvas&, QWidget* parent=0, const char* name=0, WFlags f=0);
    void clear();

    k9MenuButton* getSelected() ;
    void setMenu(k9MenuEdit *_menu) {
        m_menu=_menu;
    }

    QCanvasItem* getMoving() const;

    void setMoving(QCanvasItem* _value);
    QPtrList< k9MenuButton > *getSelection() ;
    void clearSelection();
    void hideSelection();
protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent* e);
    void updateCursor(QMouseEvent *e);
    virtual void resizeEvent ( QResizeEvent * e );
    void addSelection(QCanvasItem *_item);
    bool isSelected(QCanvasItem *_item);
    void drawSelection();
signals:
    void status(const QString&);
    void itemSelected();

private:
    QCanvasItem* moving;
    QCanvasRectangle *m_rect;
    QPoint moving_start;
    k9MenuEdit *m_menu;
    QPtrList <k9MenuButton> m_selection;
    k9CanvasSelection *m_canvasSelection;
};

#endif
