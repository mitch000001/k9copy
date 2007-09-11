//
// C++ Implementation:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9menuedit.h"
#include <qwmatrix.h>
#include <qlayout.h>
#include <kfontdialog.h>
#include "k9menubutton.h"
#include <kurlrequester.h>
#include <kcolorbutton.h>
#include <qcombobox.h>

_k9MenuEditor::_k9MenuEditor(
    QCanvas& c, QWidget* parent,
    const char* name, WFlags f) :
        QCanvasView(&c,parent,name,f) {
    this->setMaximumSize(QSize(724,580));
    moving=NULL;
}

void _k9MenuEditor::contentsMousePressEvent(QMouseEvent* e) {
    QPoint p = inverseWorldMatrix().map(e->pos());
    QCanvasItemList l=canvas()->collisions(p);
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        if ( (*it)->rtti() == 1000 ) {
//            ImageItem *item= (ImageItem*)(*it);
//            if ( !item->hit( p ) )
//                 continue;
        }

        moving = *it;
        moving_start = p;
        emit itemSelected();
        return;
    }
    moving = 0;
    emit itemSelected();
}

void _k9MenuEditor::clear() {
    QCanvasItemList list = canvas()->allItems();
    QCanvasItemList::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
        if ( *it )
            delete *it;
    }
}

void _k9MenuEditor::contentsMouseMoveEvent(QMouseEvent* e) {
    if ( moving ) {
        if (moving->rtti() !=QCanvasItem::Rtti_Text || moving==m_menu->getText()) {
            QPoint p = inverseWorldMatrix().map(e->pos());
            moving->moveBy(p.x() - moving_start.x(),
                           p.y() - moving_start.y());
            moving_start = p;
            if (moving->rtti()==1000) {
                k9CanvasSprite *spr=(k9CanvasSprite*) moving;
                spr->update();
            }
            if (moving->rtti() >1001) {
                k9CanvasSpriteRedim *sprr=(k9CanvasSpriteRedim*)moving;
                sprr->update();

            }
            if (moving==m_menu->getText())
                emit m_menu->updateTextPos(QPoint(moving->x(),moving->y()));
            canvas()->update();
        }
    }
}

k9MenuEdit::k9MenuEdit(QWidget* parent, const char* name, WFlags fl)
        : menuEdit(parent,name,fl) {
    m_format=PAL;
    m_imageHeight=576;
    QGridLayout *grid=new QGridLayout(frame,1,1);
    m_menuEditor=new _k9MenuEditor(m_canvas,frame);
    grid->addWidget(m_menuEditor,0,0);
    m_canvas.resize(720,m_imageHeight);
    m_text=new QCanvasText(&m_canvas);
    m_menuEditor->setMenu(this);
    m_text->show();
    connect(m_menuEditor,SIGNAL(itemSelected()),this,SLOT(itemSelected()));
    cbPosTitle->setEnabled(false);
}
void k9MenuEdit::updateTextPos(const QPoint &_point) {
    emit updatePos(_point);
}

void k9MenuEdit::itemSelected() {
    urBackground->setURL("");
    if (m_menuEditor->getSelected()) {
        cbColor->setColor(m_menuEditor->getSelected()->getColor());
        leTitle->setText(m_menuEditor->getSelected()->getText());
        cbPosTitle->setEnabled(true);
        cbPosTitle->setCurrentItem(m_menuEditor->getSelected()->getTextPosition()-1);
    } else {
        leTitle->setText(m_text->text());
        cbColor->setColor(m_text->color());
        cbPosTitle->setEnabled(false);
    }
}

k9MenuButton *k9MenuEdit::addButton() {
    k9MenuButton *btn= new k9MenuButton(&m_canvas);

    return btn;
}



k9MenuEdit::~k9MenuEdit() {}

/*$SPECIALIZATION$*/


void k9MenuEdit::bFontClick() {
    if (m_menuEditor->getSelected()) {
        k9MenuButton *b=m_menuEditor->getSelected();
        QFont myFont(b->getFont());
        int result = KFontDialog::getFont( myFont );
        if ( result == KFontDialog::Accepted && m_menuEditor->getSelected() ) {
            b->setFont(myFont);
        }
    } else {
        QFont myFont(m_text->font());
        int result = KFontDialog::getFont( myFont );
        if ( result == KFontDialog::Accepted  ) {
            setFont(myFont);
        }

    }
}

void k9MenuEdit::urBackgroundSelected(const QString &_fileName) {
    if ( m_menuEditor->getSelected() ) {
//If a button is selected, set the button image ...
        k9MenuButton *b=(k9MenuButton*)m_menuEditor->getSelected();
        QImage img;
        img.load(_fileName);
        b->setImage(img);
    } else {
//...otherwise, set the menu background image
        QImage img;
        img.load(_fileName);
        img=img.smoothScale(720,m_imageHeight,QImage::ScaleMax);
        setBackgroundImage(img);
    }
}

void k9MenuEdit::cbColorChanged(const QColor &_color) {
    if ( m_menuEditor->getSelected() ) {
        m_menuEditor->getSelected()->setColor(_color);
    } else {
        setColor(_color);
    }

}

void k9MenuEdit::cbPosTitleActivated(int _value) {
    if ( m_menuEditor->getSelected() ) {
        m_menuEditor->getSelected()->setTextPosition((k9MenuButton::eTextPosition)(_value+1));
    }
}

void k9MenuEdit::setBackgroundImage(const QImage &_image) {
    m_background=_image;
    m_canvas.setBackgroundPixmap(QPixmap(_image));
    emit backgroundImageChanged(_image);
}

void k9MenuEdit::leTitleChanged(const QString &_value) {
    if ( m_menuEditor->getSelected() ) {
        m_menuEditor->getSelected()->setText(_value);
    } else {
        setText(_value);
    }
}

#include "k9menuedit.moc"



k9MenuButton * _k9MenuEditor::getSelected()  {
    if (moving) {
        if (moving->rtti()==1000) {
            k9CanvasSprite *s=(k9CanvasSprite *)moving;
            return s->getButton();
        }
    }
    return NULL;
}

void k9MenuEdit::setText(const QString &_value) {
    m_text->setText(_value);
    m_canvas.update();
    emit textChanged(_value);
}

void k9MenuEdit::setFont(const QFont &_value) {
    m_text->setFont(_value);
    m_canvas.update();
    emit textFontChanged(_value);
}

void k9MenuEdit::setColor(const QColor &_value) {
    m_text->setColor(_value);
    m_canvas.update();
    emit textColorChanged(_value);
}

QCanvasText* k9MenuEdit::getText() const {
    return m_text;
}


k9MenuEdit::eFormat k9MenuEdit::getFormat() const {
    return m_format;
}


void k9MenuEdit::setFormat(const eFormat& _value) {
    m_format = _value;
    if (_value==NTSC)
        m_imageHeight=480;
    else
        m_imageHeight=576;
    m_canvas.resize(720,m_imageHeight);
    m_menuEditor->setMaximumSize(QSize(724,m_imageHeight+4));
    QImage img=m_background.smoothScale(720,m_imageHeight,QImage::ScaleMax);
    m_canvas.setBackgroundPixmap(QPixmap(img));
}


QCanvas *k9MenuEdit::getCanvas()  {
    return &m_canvas;
}
