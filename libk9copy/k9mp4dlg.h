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
#include <qimage.h>
#include <qdatetime.h>

class k9DrawImage;

class k9MP4Dlg : public MP4Dlg
{
  Q_OBJECT

public:
  k9MP4Dlg(QWidget* parent = 0, const char* name = 0);
  ~k9MP4Dlg();
  /*$PUBLIC_FUNCTIONS$*/
  void setfps(QString _fps);
  void setsize(QString _size);
  void setbitrate(QString _bitrate);
  void setremain(QString _remain);
  void setProgress(int _progress);
  void setTitleLabel(QString _titleLabel);
  void Cancel();
  void setImage(QString _fileName) ;

signals:
  /*$PUBLIC_SLOTS$*/
  void sigCancel();
private:
    k9DrawImage *m_wimage;
    QTime m_timer;
protected:
  /*$PROTECTED_FUNCTIONS$*/
};

#endif

