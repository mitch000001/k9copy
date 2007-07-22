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

#ifndef K9PROGRESS_H
#define K9PROGRESS_H

#include "k9common.h"
#include "progress.h"
#include <k9process.h>
#include <qimage.h>
#include <qtimer.h>
#include "k9drawimage.h"

class k9Progress : public Progress
{
  Q_OBJECT

public:
  k9Progress(QWidget* parent = 0, const char* name = 0, const QStringList &args=0);
  ~k9Progress();
   virtual k9Process * getProcess() const;
   virtual void setProgress(long _position,long _total);
   virtual void setLabelText(const QString _text);
   virtual void setElapsed(const QString _text);
   virtual void setTitle(const QString _text);
   virtual void setImage(QString _fileName) ;
   virtual void setMovie(QString _fileName);

   virtual int execute();
  /*$PUBLIC_FUNCTIONS$*/
   
public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/
  k9Process *m_process;
  k9DrawImage *m_wimage;
  void bCancelClick();
  bool m_canceled;
protected slots:
  /*$PROTECTED_SLOTS$*/
};

#endif

