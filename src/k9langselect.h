//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9LANGSELECT_H
#define K9LANGSELECT_H

#include "k9common.h"
#include "langselectw.h"

#include "k9main.h"
#include <qlistview.h>

class k9Main;
class k9DVD;

class ckLvLangItem : public QCheckListItem {
public:
    ckLvLangItem( QListViewItem *parent,k9Main *dlg)
            : QCheckListItem( parent,"",QCheckListItem::CheckBox) {
        mainDlg=dlg;
        streamType=NONE;
    }
    eStreamType streamType;
    k9Main *mainDlg;
    QString language;
protected:
    void stateChange(bool state);
};



class k9LangSelect : public langSelect
{
  Q_OBJECT

public:
  k9LangSelect(k9Main *mainWindow,QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9LangSelect();

  void setMain(k9Main* _value);
  void fillLvLanguages();
  void update();
  void clear();
  void selectAll(bool _state);
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/
  k9Main * m_main;
  k9DVD *m_dvd;
  QPtrList <ckLvLangItem> langAudItems;
  QPtrList <ckLvLangItem> langSubItems;
  void updateLvLang(const eStreamType streamType,const QString & lang );

protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

