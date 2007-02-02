//
// C++ Implementation: k9halconnection
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9halconnection.h"
#define DBUS_API_SUBJECT_TO_CHANGE
#include "k9haldevice.h"
#include <dbus/connection.h>
#include <dbus/dbus.h>
#include <hal/libhal.h>

k9HalConnection *Hinstance=NULL;

void halDeviceAdded (LibHalContext *ctx, const char *udi) {
   Hinstance->addDevice(udi);
   
}

void halDeviceRemoved (LibHalContext *ctx, const char *udi) {
  Hinstance->removeDevice( udi);
}


  
k9HalConnection::k9HalConnection(QObject *parent, const char *name)
 : QObject(parent, name)
{
   m_devices.setAutoDelete(true);
  m_context =(void*) libhal_ctx_new();

  DBusError error;
  dbus_error_init( &error );
  m_dbusConnect = dbus_bus_get( DBUS_BUS_SYSTEM, &error );
  if( dbus_error_is_set(&error) ) {
    qDebug(QString("Error connecting to DBUS : %1").arg(error.message));
    return;
  }
  
  m_dBusQtConnect = new DBusQt::Connection( this );
  m_dBusQtConnect->dbus_connection_setup_with_qt_main(m_dbusConnect );

  libhal_ctx_set_dbus_connection((LibHalContext*) m_context,m_dbusConnect );
  
  libhal_ctx_set_device_added( (LibHalContext*)m_context, halDeviceAdded );
  libhal_ctx_set_device_removed( (LibHalContext*)m_context, halDeviceRemoved );
  
  if( !libhal_ctx_init((LibHalContext*) m_context, 0 ) ) {
    qDebug("HAL init failed");
    return;
  }

  int numDevices;
  char** halDeviceList = libhal_get_all_devices((LibHalContext*) m_context, &numDevices, 0 );
  for( int i = 0; i < numDevices; ++i )
    //qDebug(halDeviceList[i]);
    addDevice( halDeviceList[i] );
}

void k9HalConnection::removeDevice( const char* udi ) {
   k9HalDevice *device=findDevice( udi);
   if (device !=NULL) {
	emit deviceRemoved( device);
	m_devices.remove(device);
   }
}

void k9HalConnection::addDevice( const char* udi )
{
  // ignore devices that have no property "info.capabilities" to suppress error messages
  if( !libhal_device_property_exists( (LibHalContext*) m_context, udi, "info.capabilities", 0 ) )
    return;

  if( libhal_device_query_capability( (LibHalContext*) m_context, udi, "storage.cdrom", 0 ) ) {
    char* dev = libhal_device_get_property_string( (LibHalContext*) m_context, udi, "block.device", 0 );
    if( dev ) {
      QString s( dev );
      libhal_free_string( dev );

      if( !s.isEmpty() ) {
	k9HalDevice *device=new k9HalDevice(this,udi);
	m_devices.append( device);
	emit deviceAdded( device);
      }
    }
  }
}



k9HalConnection::~k9HalConnection()
{
 libhal_ctx_shutdown((LibHalContext*)m_context, 0 );
  libhal_ctx_free ((LibHalContext*)m_context);
  delete m_dBusQtConnect;

}


k9HalConnection * k9HalConnection::getInstance() {
    if (Hinstance==NULL)
    	Hinstance=new k9HalConnection(NULL,NULL);
    return Hinstance;
}

k9HalDevice *k9HalConnection::findDevice( const char *udi) {
    for (k9HalDevice *dev= m_devices.first();dev;dev=m_devices.next()) {
        if (dev->name()==QString(udi)) 
            return dev;
    }
    return NULL;
}

void k9HalConnection::end() {
    if (Hinstance !=NULL){
    	delete Hinstance;
    }	
    Hinstance=NULL;
}
#include "k9halconnection.moc"
