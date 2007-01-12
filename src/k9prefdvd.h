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

#ifndef K9PREFDVD_H
#define K9PREFDVD_H
#include "k9common.h"
#include "prefDVD.h"

class k9prefDVD : public prefDVD
{
  Q_OBJECT

public:
  k9prefDVD(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~k9prefDVD();
  /*$PUBLIC_FUNCTIONS$*/
  void save();

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

