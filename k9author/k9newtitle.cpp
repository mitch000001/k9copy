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


#include "k9newtitle.h"
#include "k9import.h"
#include <qradiobutton.h>
#include <knuminput.h>
#include <ktimewidget.h>
#include <k9avidecode.h>
#include <klocale.h>
#include "k9menu.h"
#include "k9menubutton.h"
#include "kpushbutton.h"

k9NewTitle::k9NewTitle(QWidget* parent, const char* name, WFlags fl)
        : newTitle(parent,name,fl) {
    m_fileName="";
}

k9NewTitle::~k9NewTitle() {}

/*$SPECIALIZATION$*/

void k9NewTitle::fileSelected(const QString &_fileName) {
    m_fileName=_fileName;
    bAdd->setEnabled(true);
}

void k9NewTitle::drawImage (QImage *_image) {
    m_image=*_image;
}

void k9NewTitle::bAddClicked() {
    k9LvItemImport * item=new k9LvItemImport(m_k9Import->getRoot(),k9LvItemImport::TITLE);
    item->setOpen(true);
    k9Title *title=new k9Title( m_k9Import->getNewDVD());
    item->setText(0,i18n("title %1").arg(title->getNum() +1));

    connect( title->getButton(),SIGNAL(sigsetImage(k9MenuButton*, const QImage&)),m_k9Import,SLOT(buttonUpdated(k9MenuButton*, const QImage&)));
    item->setTitle(title);

    k9AviDecode fileInfo(0,0);
    connect(&fileInfo,SIGNAL(drawFrame(QImage*)),this,SLOT(drawImage(QImage*)));

    fileInfo.open(m_fileName);

    QTime t;
    m_k9Import->getRoot()->listView()->setColumnWidthMode(0,QListView::Maximum);
    item->setText(1,t.addSecs(fileInfo.getDuration()).toString("hh:mm:ss"));

    double increment;
    int maxCh;
    if (rbLength->isOn()) {
        QTime t;
        increment=t.secsTo(twLength->time());
        maxCh=fileInfo.getDuration()/increment;
    }

    if (rbNumber->isOn()) {
        increment=fileInfo.getDuration() /nbChapters->value();
        maxCh=nbChapters->value();
    }

    double pos=0;
    k9AviFile *last=NULL;
    int left=-80;
    int top=50;
    int width=100;
    int height=90;
    int i=-1;
    while ( pos <fileInfo.getDuration()){    
    //fileInfo.open(m_fileName);
        i++;
        fileInfo.readFrame(pos);
        fileInfo.readFrame(pos);
        k9AviFile *file=new k9AviFile(title,0);
        file->setImage(m_image);
        if (i==0)
            title->getButton()->setImage(m_image);

        file->setPrevious(last);
        if (last !=NULL)
            last->setNext(file);
        last=file;

        file->setFileName(m_fileName);
        file->setStart(t.addMSecs(pos*1000));
        pos+=increment;
        file->setEnd(t.addMSecs(QMIN(pos,fileInfo.getDuration()) *1000));
        k9LvItemImport * itemch=new k9LvItemImport(item,k9LvItemImport::CHAPTER);
        itemch->setText(0,i18n("chapter %1").arg(i+1));
        itemch->setText(1,file->getStart().toString("hh:mm:ss") +" - "+file->getEnd().toString("hh:mm:ss") );
        itemch->setAviFile(file);
        itemch->setTitle(title);
        itemch->setPixmap(0,QPixmap(m_image.smoothScale(50,50)));
        connect(file,SIGNAL(aviFileUpdated(k9AviFile*)),m_k9Import,SLOT(aviFileUpdated(k9AviFile*)));

        k9MenuButton *btn=title->getMenu()->addButton();
        //_title->setButton(btn);
        btn->setImage(m_image);
        left +=width+10;
        if (left +width >720) {
            left=30;
            top+=height+25;
        }
        btn->setTop(top);
        btn->setLeft(left);
        btn->setWidth(width);
        btn->setHeight(width);
        btn->setScript(QString("jump title 1 chapter %1 ;").arg(i+1));
        btn->setTextPosition(k9MenuButton::BOTTOM);
        btn->setText(i18n("chapter %1").arg(i+1));
        connect(file,SIGNAL(imageChanged(const QImage&)),btn,SLOT(setImage(const QImage&)));

    }
    fileInfo.close();
    m_k9Import->lvDVD->setSortColumn(0);
    m_k9Import->lvDVD->sort();
    m_k9Import->setEnableCreate(true);
}

void k9NewTitle::rbNumberClicked() {
    twLength->setEnabled(false);
    nbChapters->setEnabled(true);
}

void k9NewTitle::rbLengthClicked() {
    twLength->setEnabled(true);
    nbChapters->setEnabled(false);

}

#include "k9newtitle.moc"



void k9NewTitle::setK9Import ( k9Import* _value ) {
    m_k9Import = _value;
}
