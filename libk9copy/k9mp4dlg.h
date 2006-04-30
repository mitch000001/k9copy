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

#ifndef K9MP4DLG_H
#define K9MP4DLG_H

#include "mp4dlg.h"

class k9MP4Dlg : public MP4Dlg
{
  Q_OBJECT

public:
  k9MP4Dlg(QWidget* parent = 0, const char* name = 0);
  ~k9MP4Dlg();
  /*$PUBLIC_FUNCTIONS$*/
  void setsize(QString _size);
  void setfps(QString _fps);
  void setremain(QString _remain);
  void setProgress(int _progress);
  void accept();
public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

