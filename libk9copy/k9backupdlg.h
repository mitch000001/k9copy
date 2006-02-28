/***************************************************************************
 *   Copyright (C) 2005 by Jean-Michel Petit                               *
 *   jm_petit@laposte.net                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef K9BACKUPDLG_H
#define K9BACKUPDLG_H

#include "k9common.h"
#include "backupdlg.h"
#include "kdecmpeg2.h"
#include <qmutex.h>

class k9BackupDlg : public backupDlg
{
  Q_OBJECT

public:
  k9BackupDlg(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~k9BackupDlg();
  /*$PUBLIC_FUNCTIONS$*/
  void setTotalSteps(uint32_t _totalSteps);
  void setProgress(uint32_t _position);
  void setProgressLabel(QString _text);
  void setProgressTotal(uint32_t _position);
  void setTotalMax(uint32_t _max);
  void setFactor(QString _factor);
  bool getAbort();
  void playMovie(uchar *_data,uint32_t _size);
public slots:
  /*$PUBLIC_SLOTS$*/
private :
  bool Abort;
  QTimer *timer;
  QTime *time;
  uint64_t totalCopied,m_progressTotal;;
  QString m_progressLabel;
  uint32_t m_totalSteps,m_progress;
  QString  m_factor;
  bool m_stop;
  void update();
  kDecMPEG2 m_decoder;
  uint m_count;
  QMutex mutex;
  bool m_playmovie;
  uint m_cpt;
protected:
  /*$PROTECTED_FUNCTIONS$*/
  void bAbortClick();
  void bPlayToggled(bool state);
protected slots:
  /*$PROTECTED_SLOTS$*/
  void timerDone();
  void drawPixmap(const QImage &image);
};

#endif

