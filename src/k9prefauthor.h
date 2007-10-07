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

#ifndef K9PREFAUTHOR_H
#define K9PREFAUTHOR_H
#include "k9common.h"
#include "prefAuthor.h"
#include <qfont.h>

class k9prefAuthor : public prefAuthor
{
  Q_OBJECT

public:
  k9prefAuthor(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9prefAuthor();
  /*$PUBLIC_FUNCTIONS$*/
  void load();
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          bButtonFontClicked();
  virtual void          save();
protected:
  /*$PROTECTED_FUNCTIONS$*/
  QFont m_font;
protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

