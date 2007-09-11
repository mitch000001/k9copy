//
// C++ Implementation: k9lvitemimport
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9lvitemimport.h"


k9LvItemImport::k9LvItemImport( QListViewItem *parent,eObjectType _objectType)
            : QListViewItem( parent) {
        m_objectType=_objectType;
        m_title=NULL;
        m_aviFile=NULL;
}

int k9LvItemImport::rtti () const { 
    switch(m_objectType) {
        case TITLE:
            return 1001;
        case CHAPTER:
            return 1002;
        default:
            return 1000;
    }
}

int k9LvItemImport::compare(QListViewItem *i,int col,bool ascending) const {
 int res;
 if (i->rtti()== 1001) {
     k9LvItemImport *item2=(k9LvItemImport*)i;
     return   this->getTitle()->getNum()-item2->getTitle()->getNum();
 }
 if (i->rtti()== 1002) {
     k9LvItemImport *item2=(k9LvItemImport*)i;
     return  this->getAviFile()->getNum() - item2->getAviFile()->getNum()  ;
 }
  return QListViewItem::compare(i,col,ascending);
}


void k9LvItemImport::setTitle(k9Title* _value) {
    m_title = _value;
    QPixmap pix(m_title->getButton()->getImage());
    setPixmap(0,pix);
}

k9LvItemImport::~k9LvItemImport() {
}
