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
#include "k9menueditor.h"
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




k9MenuEdit::k9MenuEdit(QWidget* parent, const char* name,QCanvas *_canvas)
        : menuEdit(parent,name) {
    bAddText->setPixmap (SmallIcon("add"));
    m_noUpdate=false;
    m_canvas=_canvas;
    m_format=PAL;
    m_imageHeight=576;
    QGridLayout *grid=new QGridLayout(frame,1,1);
    m_menuEditor=new k9MenuEditor(*m_canvas,frame);
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
            for (b=l->first();b;b=l->next()) {
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
        for (k9MenuButton *b=l->first();b;b=l->next()) {
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
        for (k9MenuButton *b=l->first();b;b=l->next()) {
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
        for (k9MenuButton *b=l->first();b;b=l->next()) {
            QString c=_value;
            b->setText(c.replace("%n",QString::number(b->getNum())));
        }
        //m_menuEditor->getSelected()->setText(_value);
    } else {
        setText(_value);
    }
}

#include "k9menuedit.moc"





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
    disconnect (this,SIGNAL(endScriptChanged(const QString&)),0,0);
    cbEnd->setEnabled(true);
    m_menuEditor->clearSelection();
    lTitle->setText(i18n("Title %1 Menu").arg(_title->getNum()+1));
    setCanvas(_title->getMenu()->getCanvas());
    m_menuType=TITLEMENU;
    cbStart->clear();
    cbEnd->clear();
    m_startScripts.clear();
    m_endScripts.clear();


    cbStart->insertItem(i18n("Play Menu"));
    m_startScripts << "";
    cbStart->insertItem(i18n("Play Title"));
    m_startScripts << "if (g1==0) {g1=1; jump title 1 chapter 1;}";
    if (_title->getMenu()->getStartScript() !="")
        cbStart->setCurrentItem(cbStart->count()-1);
    connect (this,SIGNAL(startScriptChanged(const QString&)),_title->getMenu(),SLOT(setStartScript(const QString&)));

    cbEnd->insertItem(i18n("Play Root Menu"));
    m_endScripts << "g6=0; call vmgm menu;";
    cbEnd->insertItem(i18n("Play Title Menu"));
    m_endScripts << "call menu;";
    k9NewDVD *newDVD=(k9NewDVD*)_title->parent() ;
    k9NewDVDItems *items=newDVD->getTitles();
    for (k9Title *title=items->first();title;title=items->next()) {
        cbEnd->insertItem(i18n("Play Title %1").arg(title->getNum()+1));
        QString script=QString("g6=%1; call vmgm menu;" ).arg(title->getNum()+1);
        m_endScripts << script;
    }
    for (int i=0;i<m_endScripts.count();i++ ){
        if (_title->getMenu()->getEndScript()== *(m_endScripts.at(i)))
            cbEnd->setCurrentItem(i);
    }
    connect (this,SIGNAL(endScriptChanged(const QString&)),_title->getMenu(),SLOT(setEndScript(const QString&)));

}

void k9MenuEdit::rootSelected(k9NewDVD *_newDVD) {
    disconnect (this,SIGNAL(startScriptChanged(const QString&)),0,0);
    disconnect (this,SIGNAL(endScriptChanged(const QString&)),0,0);
    cbEnd->setEnabled(false);
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
       // QString script=QString("if (g5==0) {g5=1; jump title %1;}" ).arg(title->getNum()+1);
        QString script=QString("g6=%1;" ).arg(title->getNum()+1);
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

void k9MenuEdit::cbEndActivated (int _value) {
    emit endScriptChanged(m_endScripts[_value]);
}
