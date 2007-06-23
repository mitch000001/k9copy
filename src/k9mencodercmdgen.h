//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9MENCODERCMDGEN_H
#define K9MENCODERCMDGEN_H

#include "mencoderCmdGen.h"
#include <qscrollview.h>
#include <qgrid.h>
#include <qdom.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qhbox.h>
class _k9CheckListItem;

class k9MencoderCmdGen : public MyDialog1
{
  Q_OBJECT

public:
  k9MencoderCmdGen(QWidget* parent, const QString &_cmd, bool modal = FALSE, WFlags fl = 0 );
  ~k9MencoderCmdGen();
  /*$PUBLIC_FUNCTIONS$*/
  static bool getMencoderOptions(QString &_cmd);
public slots:
  /*$PUBLIC_SLOTS$*/
private:
   QScrollView *m_scrollView;
   QGrid *m_grid;
   QGrid *m_hbox;
   int m_row,m_page;
   QDomDocument m_doc;
   QString m_cmd;

   void addWidgets(_k9CheckListItem *_item,QString _root,QString _cat);
   void loadXml();
   void fillListView();
   void addInt(QDomElement _eOpt);
   void addFloat(QDomElement _eOpt);
   void addBool(QDomElement _eOpt);
   void addString(QDomElement _eOpt);
   void addList(QDomElement _eOpt);

   const QString & getCmd(const QString &_root);
   void selectCodec(const QString &_root,const QString &_codec);
   void parseCodecOptions(const QString &_root,const QString &_codec,const QString & _options);
   void parseCmd(const QString &_cmd);

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
    void listViewCurrentChanged(QListViewItem*);
};

#endif

