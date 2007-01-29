//
// C++ Implementation: k9cddrive
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9common.h"
#include "k9cddrive.h"
#include "k9config.h"
#include "k9halconnection.h"
#include "k9haldevice.h"
#include <kprocess.h>
#ifdef Q_OS_FREEBSD
   #include <sys/param.h>
#endif
#include <sys/mount.h>
k9CdDrive::k9CdDrive() { 
    canReadDVD=false;
    canWriteCDR=false;
    canWriteDVD=false;
    device="";
    name="";
    m_Device=NULL;
}
k9CdDrive::~k9CdDrive() {}

k9CdDrives::k9CdDrives() {
    drives.setAutoDelete(true);
    m_connection=k9HalConnection::getInstance();
    scanDrives();
}
k9CdDrives::~k9CdDrives() {
    m_connection->end();
}

void k9CdDrives::eject(const QString & device) {
    KProcess *process =new KProcess();
    *process <<"eject" << device;
    process->start();
    process->wait();
    delete process;
}


/** reads devices that was entered manually **/
void k9CdDrives::readConfig() {
    QStringList ldev;
    QStringList llabels;
    QStringList lIO;
    k9Config config;
    ldev=config.getDevices();
    llabels=config.getDevicesLabels();
    lIO=config.getDevicesIO();
    int row=count();
    int i=0;
    for ( QStringList::Iterator it = ldev.begin(); it != ldev.end(); ++it ) {
        k9CdDrive *drive=new k9CdDrive;
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
            drive->canWriteDVD=true;
        }
        drive->num=row;
        drives.append(drive);
        row++;
        i++;
    }
}

/** No descriptions */
void k9CdDrives::scanDrives() {
    drives.clear();

    QPtrList <k9HalDevice> list=m_connection->getDevices();
	
    for (k9HalDevice *hdrive=list.first();hdrive;hdrive=list.next()) {
    
        k9CdDrive *drive=new k9CdDrive;
        drive->setDevice( hdrive);
        drive->canReadDVD=hdrive->getCanReadDvd();
        drive->canWriteDVD=hdrive->getCanBurnDvd();
        drive->canWriteCDR=hdrive->getCanBurnCd();
        drive->device=hdrive->getDeviceName();
        drive->name=hdrive->getModel();
        QValueList <int> writeSpeeds;
        for (int i=2;i <=hdrive->getMaxWriteSpeed()/1385;i+=2)
            writeSpeeds.append( i);
        drive->setWriteSpeeds(writeSpeeds);
        drives.append(drive);    
    }

    readConfig();
}


/** No descriptions */
int k9CdDrives::count() {
    return drives.count();
}

/** No descriptions */
k9CdDrive * k9CdDrives::getDrive(int num) {
    return (k9CdDrive *)drives.at(num);
}


QValueList< int > k9CdDrive::getWriteSpeeds() const {
    return writeSpeeds;
}


void k9CdDrive::setWriteSpeeds(const QValueList< int >& _value) {
    writeSpeeds = _value;
}



#include "k9cddrive.moc"
