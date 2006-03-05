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

#ifndef KCDDRIVE_H
#define KCDDRIVE_H

#include "k9common.h"
#include "kconfigdlg.h"
#include "k3bdevice.h"
#include "k3bdevicemanager.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>


/**
  *@author 
  */

class kCDDrive : public QObject {
    Q_OBJECT
public:
    kCDDrive();
    ~kCDDrive();
    int num;
    QString name;
    QString device;
    bool canWriteCDR;
    bool canWriteCDRW;
    bool canReadDVD;
    bool canWriteDVD;
    K3bDevice::Device *getk3bDev() {return k3bDev;};
    void setk3bDev(K3bDevice::Device *_dev) {k3bDev=_dev;};

    void setWriteSpeeds(const QValueList< int >& _value);
    QValueList< int > getWriteSpeeds() const;
	
private:
   QValueList <int> writeSpeeds;
   K3bDevice::Device *k3bDev;
};

class kCDDrives : public QObject  {
    Q_OBJECT
public:
    kCDDrives();
    ~kCDDrives();
    /** No descriptions */
    int count();
    /** No descriptions */
    kCDDrive * getDrive(int num);
    void eject(const QString & device);
    void scanDrives();

private: // Private methods
    /** No descriptions */
     K3bDevice::DeviceManager  *dm;

    void readConfig();
    QObjectList drives;
};


#endif
