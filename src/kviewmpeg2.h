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

#include "kdecmpeg2.h"
#include "k9dvdtitle.h"

#include <viewmpeg2.h>
#include <qstring.h>
/**
  *@author 
  */

class kViewMPEG2 : public ViewMPEG2  {
Q_OBJECT
public: 
	kViewMPEG2();
	~kViewMPEG2();
  int open (const QString & device,k9DVDTitle * title);
private:
  k9DVDTitle *m_title;
  QString dev;
  int selTitle;
  unsigned long  idxLect;
  bool pause;
  bool lockSlider;
  QImage img;
  bool stopped;
  void setError(const QString & err);
  void bPlayClick();
  void bStopClick();
  void sliderReleased();  
  void playDVD(dvd_reader_t *dvd,int titleSetNr);
  /** No descriptions */
  void sliderPressed();
  kDecMPEG2 *decoder;
  QString file;
  QString errMsg;
  bool error;
  QString length;
  uint startSector;
  uint lastSector;
protected:
 void closeEvent( QCloseEvent* );
public slots: // Public slots
  /** No descriptions */
  void drawPixmap(const QImage &image);

};

#endif
