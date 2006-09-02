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

#include "kcddrive.h"
#include <kprocess.h>
#ifdef Q_OS_FREEBSD
   #include <sys/param.h>
#endif
#include <sys/mount.h>
kCDDrive::kCDDrive() {
    canReadDVD=false;
    canWriteCDR=false;
    canWriteCDRW=false;
    canWriteDVD=false;
    device="";
    name="";
    k3bDev=NULL;
}
kCDDrive::~kCDDrive() {}

kCDDrives::kCDDrives() {
    dm= new K3bDevice::DeviceManager(this);
    drives.setAutoDelete(true);
    scanDrives();
}
kCDDrives::~kCDDrives() {
    dm->clear();
    delete dm;
}

void kCDDrives::eject(const QString & device) {
    bool ejected=FALSE;
    for (uint i=0;i<count();i++) {
        kCDDrive *drive=getDrive(i);
        if ((drive->device==device) && ( drive->getk3bDev()!=NULL) ) {
            K3bDevice::Device *dev=drive->getk3bDev();
            /*KProcess *process =new KProcess();
            *process <<"umount" << device;
            process->start();
            if (process->isRunning()) process->wait(-1);
            delete process;
            */

#ifdef Q_OS_LINUX

            umount(device.latin1());
#endif
#ifdef Q_OS_FREEBSD

            unmount(device.latin1(),0);
#endif

            dev->eject();
	    ejected=TRUE;
        }
    }

    if (!ejected) {
	   KProcess *process =new KProcess();
           *process <<"eject" << device;
           process->start();
	   process->wait();
	   delete process;
    }
}


/** reads devices that was entered manually **/
void kCDDrives::readConfig() {
    QStringList ldev;
    QStringList llabels;
    QStringList lIO;
    KSimpleConfig settings("K9Copy");
    ldev=settings.readListEntry("devices/dev");
    llabels=settings.readListEntry("devices/labels");
    lIO=settings.readListEntry("devices/IO");
    int row=count();
    int i=0;
    for ( QStringList::Iterator it = ldev.begin(); it != ldev.end(); ++it ) {
        kCDDrive *drive=new kCDDrive;
        drive->device=(*it).latin1();
        QStringList::Iterator it2=llabels.at(i);
        QStringList::Iterator it3=lIO.at(i);
        drive->name=(*it2).latin1();
        QString c=(*it3).latin1();
        if (c.contains("I")) {
            drive->canReadDVD=true;
        }
        if (c.contains("O")) {
            drive->canWriteCDR=true;
            drive->canWriteCDRW=true;
            drive->canWriteDVD=true;
        }
        drive->num=row;
        drives.append(drive);
        row++;
        i++;
    }
}

/** No descriptions */
void kCDDrives::scanDrives() {
    int i;
    drives.clear();
    dm->clear();
    dm->scanBus();
    dm->scanFstab();


    QPtrListIterator<K3bDevice::Device> it( dm->allDevices());
    while( it.current() ) {
        kCDDrive *drive=new kCDDrive;
        drive->setk3bDev(it.current());
        drive->canReadDVD=it.current()->readsDvd();
        drive->canWriteDVD=it.current()->writesDvd();
        drive->canWriteCDR=it.current()->writesCd();
        drive->canWriteCDRW=it.current()->writesCdrw();
        ;
        drive->device=it.current()->devicename();
        drive->name=it.current()->description();
        drive->setWriteSpeeds(it.current()->determineSupportedWriteSpeeds());
        drives.append(drive);
        //ajouter les vitesses autoris�
        ++it;

    }

    readConfig();
}

/** No descriptions */
int kCDDrives::count() {
    return drives.count();
}

/** No descriptions */
kCDDrive * kCDDrives::getDrive(int num) {
    return (kCDDrive *)drives.at(num);
}


QValueList< int > kCDDrive::getWriteSpeeds() const {
    return writeSpeeds;
}


void kCDDrive::setWriteSpeeds(const QValueList< int >& _value) {
    writeSpeeds = _value;
}
