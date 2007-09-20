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

#include "k9common.h"
#include "k9menuedit.h"
#include "k9menu.h"
#include "k9title.h"
#include "k9newdvd.h"
#include <qwmatrix.h>
#include <qlayout.h>
#include <kfontdialog.h>
#include "k9menubutton.h"
#include <kurlrequester.h>
#include <kcolorbutton.h>
#include <qcombobox.h>
#include <klocale.h>
#include <qlabel.h>
#include <kiconloader.h>

_k9MenuEditor::_k9MenuEditor(
    QCanvas& c, QWidget* parent,
    const char* name, WFlags f) :
        QCanvasView(&c,parent,name,f) {
    this->setMaximumSize(QSize(724,580));
    moving=NULL;
    m_rect=NULL;
}


void _k9MenuEditor::contentsMouseReleaseEvent(QMouseEvent* e) {
  if (m_rect) {
        m_rect->hide();
        delete m_rect;
        m_rect=NULL;
        canvas()->update();
   }
    
}

void _k9MenuEditor::contentsMousePressEvent(QMouseEvent* e) {
    QPoint p = inverseWorldMatrix().map(e->pos());
    moving_start = p;
    QCanvasItemList l=canvas()->collisions(p);
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
        } else if (moving->rtti() <1001)
            clearSelection();
        emit itemSelected();
        return;
    }
    moving = 0;
    if (m_rect==NULL)
        m_rect=new QCanvasRectangle(this->canvas());
    else
        m_rect->setCanvas(this->canvas());
    m_rect->setPen(QPen(Qt::red));
    m_rect->setX(p.x());
    m_rect->setY(p.y());
    m_rect->setSize(0,0);
    clearSelection();
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

void _k9MenuEditor::clearSelection() {
    for (k9MenuButton *b=m_selection.first();b;b=m_selection.next())
        b->select(false);
    m_selection.clear();
}

bool _k9MenuEditor::isSelected(QCanvasItem *_item) {
    k9CanvasSprite *s=(k9CanvasSprite *)_item;
    k9MenuButton *button=s->getButton();
    if (m_selection.find(button) !=-1) 
        return true;
    else   
        return false;
}

void _k9MenuEditor::addSelection(QCanvasItem *_item) {
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



void _k9MenuEditor::contentsMouseMoveEvent(QMouseEvent* e) {
    if ( moving ) {
        if (moving->rtti() !=QCanvasItem::Rtti_Text || moving==m_menu->getText()) {
            QPoint p = inverseWorldMatrix().map(e->pos());
	    int offsetX=p.x() - moving_start.x();
	    int offsetY=p.y() - moving_start.y();
            moving_start = p;
            if (moving->rtti()==1000) {
		for (k9MenuButton *b=m_selection.first();b;b=m_selection.next()) {
			k9CanvasSprite*spr=b->getSprite();
			spr->moveBy(offsetX,offsetY);
			spr->update();
		}
			
            } else
            	moving->moveBy(offsetX,offsetY);

            if (moving->rtti() >1001) {
                k9CanvasSpriteRedim *sprr=(k9CanvasSpriteRedim*)moving;
                sprr->update();

            }
            if (moving==m_menu->getText())
                emit m_menu->updateTextPos(QPoint(moving->x(),moving->y()));
            canvas()->update();
        }
    } else {
        m_rect->hide();
        canvas()->update();
        QPoint p = inverseWorldMatrix().map(e->pos());
	int offsetX=p.x() - moving_start.x();
	int offsetY=p.y() - moving_start.y();        
        m_rect->setSize(offsetX,offsetY);
        m_rect->show();
        canvas()->update();
        clearSelection();
        QCanvasItemList l=canvas()->collisions(m_rect->rect());
        for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
            QCanvasItem *item = *it;
            if (item->rtti()==1000) {
                addSelection(item);
            }
        } 
    }
}

void _k9MenuEditor::resizeEvent ( QResizeEvent * e ) {
    QWMatrix m;
    double scalex=(e->size().width()-4.0)/720.0;
    double scaley=(e->size().height()-4.0)/576.0;
    m.scale(QMIN(scalex,scaley),QMIN(scalex,scaley));
    this->setWorldMatrix(m);

}


k9MenuEdit::k9MenuEdit(QWidget* parent, const char* name,QCanvas *_canvas)
        : menuEdit(parent,name) {
    bAddText->setPixmap (SmallIcon("add"));
    m_noUpdate=false;
    m_canvas=_canvas;
    m_format=PAL;
    m_imageHeight=576;
    QGridLayout *grid=new QGridLayout(frame,1,1);
    m_menuEditor=new _k9MenuEditor(*m_canvas,frame);
    grid->addWidget(m_menuEditor,0,0);
    m_canvas->resize(720,m_imageHeight);
    m_text=NULL;
    m_menuEditor->setMenu(this);
    connect(m_menuEditor,SIGNAL(itemSelected()),this,SLOT(itemSelected()));
    cbPosTitle->setEnabled(false);
}
void k9MenuEdit::updateTextPos(const QPoint &_point) {
    emit updatePos(_point);
}

void k9MenuEdit::itemSelected() {
    bool unselect=true;
   m_noUpdate=true;
   m_canvas->update(); 
   urBackground->setURL("");
    if (m_menuEditor->getSelected()) {
   //     m_menuEditor->getSelected()->select(true);
        cbColor->setColor(m_menuEditor->getSelected()->getColor());
        leTitle->setText(m_menuEditor->getSelected()->getText());
        cbPosTitle->setEnabled(true);
        cbPosTitle->setCurrentItem(m_menuEditor->getSelected()->getTextPosition()-1);
        urBackground->setEnabled(true);
        m_canvas->update();
    } else if (m_menuEditor->getMoving()) {
        if (m_menuEditor->getMoving()->rtti()==QCanvasItem::Rtti_Text) {
            m_text=(QCanvasText*)m_menuEditor->getMoving();
            leTitle->setText(m_text->text());
            cbColor->setColor(m_text->color());
            cbPosTitle->setEnabled(false);
            urBackground->setEnabled(false);
        }
    } else {
        urBackground->setEnabled(true);
        cbPosTitle->setEnabled(false);
    }
    m_noUpdate=false;
}





k9MenuEdit::~k9MenuEdit() {}

/*$SPECIALIZATION$*/


void k9MenuEdit::bFontClick() {
    if (m_menuEditor->getSelected()) {
        k9MenuButton *b=m_menuEditor->getSelected();
        QFont myFont(b->getFont());
        int result = KFontDialog::getFont( myFont );
        if ( result == KFontDialog::Accepted && m_menuEditor->getSelected() ) {
		QPtrList <k9MenuButton> *l=m_menuEditor->getSelection();
		for (b=l->first();b;b=l->next()){
			b->setFont(myFont);
		}
        }
    } else {
        if (m_text) {
            QFont myFont(m_text->font());
            int result = KFontDialog::getFont( myFont );
            if ( result == KFontDialog::Accepted  ) 
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
    if (m_noUpdate)
        return;
    if ( m_menuEditor->getSelected() ) {
	QPtrList <k9MenuButton> *l=m_menuEditor->getSelection();
	for (k9MenuButton *b=l->first();b;b=l->next()){
		b->setColor(_color);
	}
        //m_menuEditor->getSelected()->setColor(_color);
    } else {
        setColor(_color);
    }

}

void k9MenuEdit::cbPosTitleActivated(int _value) {
    if (m_noUpdate)
        return;
    if ( m_menuEditor->getSelected() ) {
	QPtrList <k9MenuButton> *l=m_menuEditor->getSelection();
	for (k9MenuButton *b=l->first();b;b=l->next()){
		b->setTextPosition((k9MenuButton::eTextPosition)(_value+1));
	}
    }
}

void k9MenuEdit::setBackgroundImage(const QImage &_image) {
    m_background=_image;
    m_canvas->setBackgroundPixmap(QPixmap(_image));
    emit backgroundImageChanged(_image);
}

void k9MenuEdit::leTitleChanged(const QString &_value) {
    if (m_noUpdate)
        return;
    if ( m_menuEditor->getSelected() ) {
	QPtrList <k9MenuButton> *l=m_menuEditor->getSelection();
	for (k9MenuButton *b=l->first();b;b=l->next()){
		b->setText(_value);
	}
        //m_menuEditor->getSelected()->setText(_value);
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
    if (m_text)
        m_text->setText(_value);
    m_canvas->update();
    emit textChanged(_value);
}

void k9MenuEdit::setFont(const QFont &_value) {
    if (m_text)
        m_text->setFont(_value);
    m_canvas->update();
    emit textFontChanged(_value);
}

void k9MenuEdit::setColor(const QColor &_value) {
    if (m_text)
        m_text->setColor(_value);
    m_canvas->update();
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
    QPixmap pix=m_canvas->backgroundPixmap();
    QImage img=pix.convertToImage().smoothScale(720,m_imageHeight);
    m_canvas->resize(720,m_imageHeight);
    m_menuEditor->setMaximumSize(QSize(724,m_imageHeight+4));
    m_canvas->setBackgroundPixmap(QPixmap(img));
}


QCanvas *k9MenuEdit::getCanvas()  {
    return m_canvas;
}


void k9MenuEdit::setCanvas(QCanvas* _value) {
    m_canvas = _value;
    m_text=NULL;
    m_menuEditor->setCanvas(_value);
    m_canvas->resize(720,m_imageHeight);
    m_canvas->update();
    m_menuEditor->updateContents();
}

void k9MenuEdit::titleSelected(k9Title *_title) {
    disconnect (this,SIGNAL(startScriptChanged(const QString&)),0,0);
    m_menuEditor->clearSelection();
    lTitle->setText(i18n("Title %1 Menu").arg(_title->getNum()+1));
    setCanvas(_title->getMenu()->getCanvas());
    m_menuType=TITLEMENU;
    cbStart->clear();
    m_startScripts.clear();
    cbStart->insertItem(i18n("Play Menu"));
    m_startScripts << "";
    cbStart->insertItem(i18n("Play Title"));
    m_startScripts << "if (g1==0) {g1=1; jump title 1 chapter 1;}";
    if (_title->getMenu()->getStartScript() !="")
       cbStart->setCurrentItem(cbStart->count()-1);
    connect (this,SIGNAL(startScriptChanged(const QString&)),_title->getMenu(),SLOT(setStartScript(const QString&)));
}

void k9MenuEdit::rootSelected(k9NewDVD *_newDVD) {
    disconnect (this,SIGNAL(startScriptChanged(const QString&)),0,0);
    m_menuEditor->clearSelection();
    lTitle->setText(i18n("Root Menu"));
    setCanvas(_newDVD->getRootMenu()->getCanvas());
    m_menuType=ROOTMENU;
    cbStart->clear();
    m_startScripts.clear();
    cbStart->insertItem(i18n("Play Menu"));
    m_startScripts << "";
    k9NewDVDItems *items=_newDVD->getTitles();
    for (k9Title *title=items->first();title;title=items->next()) {
        cbStart->insertItem(i18n("Play Title %1").arg(title->getNum()+1));
        QString script=QString("if (g5==0) {g5=1; jump title %1;}" ).arg(title->getNum()+1);
        m_startScripts << script;
        if (script==_newDVD->getRootMenu()->getStartScript())
            cbStart->setCurrentItem(cbStart->count()-1);
    }
    connect (this,SIGNAL(startScriptChanged(const QString&)),_newDVD->getRootMenu(),SLOT(setStartScript(const QString&)));
}

void k9MenuEdit::bAddTextClick() {
    m_text=new QCanvasText(m_canvas);
    m_text->show();
    m_menuEditor->setMoving(m_text);
    setText(leTitle->text());
    setColor(cbColor->color());
}

void k9MenuEdit::cbStartActivated (int _value) {
    emit startScriptChanged(m_startScripts[_value]);
}

QCanvasItem* _k9MenuEditor::getMoving() const {
    return moving;
}


void _k9MenuEditor::setMoving(QCanvasItem* _value) {
    moving = _value;
}



QPtrList< k9MenuButton > *_k9MenuEditor::getSelection()  {
    return &m_selection;
}
