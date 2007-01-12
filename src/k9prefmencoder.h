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

#ifndef K9PREFMENCODER_H
#define K9PREFMENCODER_H

#include "k9common.h"
#include "prefMencoder.h"
#include <qstringlist.h>
#include <qstring.h>

class k9prefMencoder : public prefMencoder
{
  Q_OBJECT

public:
  k9prefMencoder(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9prefMencoder();
  /*$PUBLIC_FUNCTIONS$*/
  void save(void);
public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void bAddClick();
  void bRemoveClick();
  QString optValue(QString _value);
};

#endif

