//
// C++ Implementation: k9title
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9title.h"
#include "k9newdvd.h"
#include "k9menu.h"

k9Title::k9Title(k9NewDVD *parent, const char *name)
 : QObject(parent, name)
{
    m_num=parent->getTitles()->count();
    parent->appendTitle(this);
    m_menu=new k9Menu(this,0);
    m_menu->setFormat((k9Menu::eFormat)parent->getFormat());
}


k9Title::~k9Title()
{
}

int k9TitleItems::compareItems(QPtrCollection::Item item1,QPtrCollection::Item item2) {
    k9AviFile *i1,*i2;
    i1=(k9AviFile*) item1;
    i2=(k9AviFile*) item2;
    return (i1->getNum() - i2->getNum());
}

#include "k9title.moc"


k9TitleItems *k9Title::getFiles() 
{
	return &m_files;
}


int k9Title::getNum() const
{
	return m_num;
}


k9MenuButton* k9Title::getButton() const {
    return m_button;
}


void k9Title::setButton(k9MenuButton* _value) {
    m_button = _value;
}


k9Menu* k9Title::getMenu() const {
    return m_menu;
}
