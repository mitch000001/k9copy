//
// C++ Interface: k9title
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9TITLE_H
#define K9TITLE_H

#include "k9common.h"
#include <qobject.h>
#include <qptrlist.h>
#include "k9avifile.h"
#include "k9menubutton.h"
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9NewDVD;

class k9TitleItems : public QPtrList<k9AviFile> {
protected:
    virtual int compareItems(QPtrCollection::Item item1,QPtrCollection::Item item2);
};

class k9Title : public QObject {
    Q_OBJECT
public:
    k9Title(k9NewDVD *parent = 0, const char *name = 0);
    ~k9Title();

    k9TitleItems *getFiles();

    int getNum() const;

    void setButton(k9MenuButton* _value);


    k9MenuButton* getButton() const;


private:
    k9TitleItems m_files;
    int m_num;
    k9MenuButton *m_button;
};

#endif
