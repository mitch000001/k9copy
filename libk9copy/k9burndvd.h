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
#ifndef k9BurnDVD_H
#define k9BurnDVD_H

#include "k9common.h"
#include "k9progress.h"

#include <k9process.h>
#include <klocale.h>

/**
@author Jean-Michel PETIT
*/
class k9BurnDVD: public QObject
{
Q_OBJECT
public:
  k9BurnDVD();

  ~k9BurnDVD();
  virtual void setburnDevice( const QString& _newVal);
  virtual const QString& getburnDevice();
  virtual QWidget* getDialog() {return progress;};
  void setworkDir( const QString& _newVal);
  void setvolId(QString  _newVal);
  void setUseK3b(bool _newVal);
  void setAutoBurn (bool _newVal);
  void burn();
  void makeIso(QString _filename);

  void setSpeed(const QString& _value) { m_speed = _value;}
  
private:
    QTime *time;
    k9Progress *progress;
    k9Process *proc;
    k9Process *proc2;
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
    bool m_cdrkit;
    void burnWithGrowisofs();
    void burnWithGenisoimage();
    void getGenisoimageCmd(k9Process *proc,QString _fileName,bool _printSize);
    void getWodimCmd(k9Process *proc);

    void burnWithK3b();
    const QString &getImageSize();
    QString imageSize;
    QString m_speed;
private slots: // Private slots    
    void growisoStderr(KProcess *proc, char *buffer, int buflen);
    void growisoStdout(KProcess *proc, char *buffer, int buflen);  
    void mkisoSizeStderr(KProcess *proc, char *buffer, int buflen);
    void mkisoSizeStdout(KProcess *proc, char *buffer, int buflen);
};

#endif
