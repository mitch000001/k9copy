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
#ifndef KBURNDVD_H
#define KBURNDVD_H

#include "k9common.h"
#include <kprogress.h>
#include <qprocess.h>
#include <klocale.h>


/**
@author Jean-Michel PETIT
*/
class kBurnDVD: public QObject
{
Q_OBJECT
public:
  kBurnDVD();

  ~kBurnDVD();
  virtual void setburnDevice( const QString& _newVal);
  virtual const QString& getburnDevice();
  void setworkDir( const QString& _newVal);
  void setvolId(QString  _newVal);
  void setUseK3b(bool _newVal);
  void setAutoBurn (bool _newVal);
  void burn();
  void makeIso(QString _filename);

  void setSpeed(const QString& _value) { m_speed = _value;}
  
private:
    KProgressDialog *progress;
    QProcess *proc;
    QProcess *proc2;
    QString volId;
    float burnSpeed;
    QString burnDevice;
    QString lastMsg;
    QString workDir;
    QString m_filename;
    bool cancelled;
    bool useK3b;
    bool autoBurn;
    bool iso;
    void burnWithGrowisofs();
    void burnWithK3b();
    const QString &getImageSize();
    QString imageSize;
    QString m_speed;
private slots: // Private slots    
    void growisoStderr();
    void growisoStdout();  
    void mkisoSizeStderr();
    void mkisoSizeStdout();
};

#endif
