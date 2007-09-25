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
#include "k9tools.h"

#ifdef HAVE_HAL
#include "k9halconnection.h"
#include "k9haldevice.h"
#else
#include <k3bdevice.h>
#include <k3bdevicemanager.h>
#endif

#include <kprocess.h>
k9CdDrive::k9CdDrive() { 
    canReadDVD=false;
    canWriteCDR=false;
    canWriteDVD=false;
    device="";
    name="";
    #ifdef HAVE_HAL
    m_Device=NULL;
    #endif
}
k9CdDrive::~k9CdDrive() {}

k9CdDrives::k9CdDrives():QObject( 0,0) {
    drives.setAutoDelete(true);
    #ifdef HAVE_HAL
    m_connection=k9HalConnection::getInstance();
    connect(m_connection,SIGNAL(deviceAdded( k9HalDevice* )),this,SLOT(deviceAdded( k9HalDevice* )));
    connect(m_connection,SIGNAL(deviceRemoved( k9HalDevice* )),this,SLOT(deviceRemoved( k9HalDevice*)));
    #else
        m_devMgr=new K3bDevice::DeviceManager(this);

    #endif
    scanDrives();
}
k9CdDrives::~k9CdDrives() {
    #ifdef HAVE_HAL
    m_connection->end();
    #else
    delete m_devMgr;
    #endif
}

void k9CdDrives::deviceAdded( k9HalDevice *_device) {
#ifdef HAVE_HAL
   addDrive( _device);
#endif
}

void k9CdDrives::deviceRemoved(k9HalDevice *_device) {
#ifdef HAVE_HAL   
   for (k9CdDrive *d=drives.first();d;d=drives.next()) {
      if (d->getDevice()==_device) {
      	  emit deviceRemoved( d);
          drives.remove(d);
          break;
      }
   }
#endif
}
#ifdef HAVE_HAL
void k9CdDrives::addDrive(k9HalDevice *_device) {
       k9CdDrive *drive=new k9CdDrive;
        drive->setDevice( _device);
        drive->canReadDVD=_device->getCanReadDvd();
        drive->canWriteDVD=_device->getCanBurnDvd();
        drive->canWriteCDR=_device->getCanBurnCd();
        drive->device=_device->getDeviceName();
        drive->name=_device->getModel();
        QValueList <int> writeSpeeds;
        for (int i=2;i <=_device->getMaxWriteSpeed()/1385;i+=2)
            writeSpeeds.append( i);
        drive->setWriteSpeeds(writeSpeeds);
        drives.append(drive);    
        emit deviceAdded( drive);
 
}
#endif

/** No descriptions */
void k9CdDrives::scanDrives() {
    drives.clear();

    #ifdef HAVE_HAL
    QPtrList <k9HalDevice> list=m_connection->getDevices();
	
    for (k9HalDevice *hdrive=list.first();hdrive;hdrive=list.next()) {
        addDrive(hdrive);
    }
    #else
      m_devMgr->scanBus();
      int row=0;
      QPtrList <K3bDevice::Device> lDev=m_devMgr->allDevices();
      for (K3bDevice::Device *dev=lDev.first();dev;dev=lDev.next()) {
         k9CdDrive *drive=new k9CdDrive;
         drive->device=dev->blockDeviceName();
         drive->name=dev->description();
         drive->canReadDVD=dev->readsDvd();
         drive->canWriteCDR=dev->writesCd();
         drive->canWriteDVD=dev->writesDvd();
         QValueList <int> writeSpeeds;
         for (int i=2;i <=dev->determineMaximalWriteSpeed()/1385;i+=2)
            writeSpeeds.append( i);
         drive->setWriteSpeeds(writeSpeeds);
         
        drive->num=row;
        drives.append(drive);
        row++;
        emit deviceAdded(drive);      
      }
      
    #endif
    readConfig();
}

void k9CdDrives::eject(const QString & device) {
    KProcess *process =new KProcess();
    if (k9Tools::checkProgram("kdeeject"))
        *process <<"kdeeject" << device;
    else    
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
        drive->device=(*it);
        QStringList::Iterator it2=llabels.at(i);
        QStringList::Iterator it3=lIO.at(i);
        drive->name=(*it2);
        QString c=(*it3);
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
        emit deviceAdded(drive);
    }
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
