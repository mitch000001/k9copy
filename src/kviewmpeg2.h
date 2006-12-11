/**************************************************************************
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

#ifndef KVIEWMPEG2_H
#define KVIEWMPEG2_H

#include "k9dvdtitle.h"

#include <qthread.h>
#include <viewmpeg2.h>
#include <qwidget.h>
#include <qstring.h>
#include <qevent.h>
#include <qmutex.h>
#include <qgl.h>
#include "k9plaympeg2.h"
#include "k9glwidget.h"
#include "k9mplayer.h"

/**
  *@author 
  */

class k9Widget : public QWidget {
Q_OBJECT
protected:
   QImage *m_image;
   void paintEvent ( QPaintEvent * _event);
public:
   k9Widget (QWidget *_parent);
   void setImage(QImage *_image);
};


class kViewMPEG2 : public ViewMPEG2  {
Q_OBJECT
public: 
	kViewMPEG2();
	~kViewMPEG2();
private:
  QMutex mutex;
  k9DVDTitle *m_title;
  QString dev;
  int selTitle;
  unsigned long  idxLect;
  bool pause;
  bool lockSlider;
  QImage img;
  bool stopped;
  void sliderReleased();  
  /** No descriptions */
  void sliderPressed();
  void lock();
  void unlock();
  QString file;
  QString errMsg;
  bool error;
  QString length;
  uint startSector;
  uint lastSector;
  k9PlayMPEG2 m_player;
  QGridLayout *m_layout;
  bool m_prefUseGL;
  k9GLWidget *m_GLwidget;
  k9Widget *m_widget;
protected:
 void closeEvent( QCloseEvent* );
 void resizeEvent ( QResizeEvent * );
public slots: // Public slots
  /** No descriptions */
  int open (k9DVD *_dvd,k9DVDTitle * title,int chapter);
  void drawPixmap( QImage *image);
  void drawppm(uchar *_buf,int _width,int _height,int _len);
  void setError(const QString & err);
  void setPosition(uint32_t _position);
  void setMax(uint32_t _position);
  void setMin(uint32_t _position);
  void bPlayClick();
  void bStopClick();
  void readSettings();

};

#endif
