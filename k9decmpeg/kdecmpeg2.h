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

#ifndef KDECMPEG2_H
#define KDECMPEG2_H
#include "k9common.h"
#include "mpeg2.h"
#include <stdio.h>
#include <stdlib.h>
#include <qimage.h>
#include "ac.h"
#include <qmutex.h>
#include <qwidget.h>
#include <qobject.h>
#include <qthread.h>
/**
  *@author 
  */

class kDecMPEG2;

class k9DisplayThread:public QThread {

public:
   k9DisplayThread(kDecMPEG2 *_dec) {m_dec=_dec;}
   void setImage(QImage _image);
protected:
   kDecMPEG2 *m_dec;
   QImage m_image;
   QMutex m_mutex;
   void run();

};



class kDecMPEG2 : public QObject  {
Q_OBJECT
public: 
  kDecMPEG2();
  ~kDecMPEG2();
  int decode (uint8_t * buf, uint8_t * end, int flags);
  void restart();
  void start();
  void stop();
  void draw(QImage *image) {emit pixmapReady(image);}
private:
  QImage pix;
  bool m_opened;
  int demux_pid;
  int demux_track;
  QMutex mutex;
  mpeg2dec_t * decoder;
  k9DisplayThread *m_display;
  int demux (uint8_t * buf, uint8_t * end, int flags);
  void save_ppm (int width, int height, uint8_t * buf, int num);
  void decode_mpeg2(uint8_t * current, uint8_t * end);
 signals: // Signals
  /** No descriptions */
  void pixmapReady(QImage *image);
  void ppmReady(uchar *buffer,char * data,int size);
};                       

#endif
