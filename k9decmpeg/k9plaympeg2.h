//
// C++ Interface: k9plaympeg2
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9PLAYMPEG2_H
#define K9PLAYMPEG2_H

#include "k9common.h"
#include "k9dvdtitle.h"


#include <qthread.h>
#include <qobject.h>
#include <qmutex.h>
#include <qwidget.h>
#include "k9decodethread.h"
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9PlayMPEG2 : public QObject,public QThread
{
Q_OBJECT
private:
   k9DVDTitle *m_title;
   QString m_device;
   k9DecodeThread m_decoder;
   uint32_t m_idxLect;
   uint32_t m_startSector;
   uint32_t m_lastSector;
   bool m_stopped;
   dvd_file_t *dvdfile;
   QMutex mutex;
public:
    k9PlayMPEG2();
    void open (const QString & device,k9DVDTitle * title);
    kDecMPEG2 *getDecoder();

    ~k9PlayMPEG2();
protected:
    void run();
    void playTitle();
public slots:
  void stop();
  void play();
  void updatePos(uint32_t _position);	
signals:
  void setPosition(uint32_t _position);
  void setMin(uint32_t _sector);
  void setMax(uint32_t _sector);

  void setError(const QString & err);

};

#endif
