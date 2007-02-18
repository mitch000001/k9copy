//
// C++ Interface: k9halconnection
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
#ifdef HAVE_HAL
#ifndef K9HALCONNECTION_H
#define K9HALCONNECTION_H

#include <qobject.h>
#include <qptrlist.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9HalDevice;
class DBusConnection;
#ifdef DBUS_QT3
class QDBusConnection;
#else
namespace DBusQt {
  class Connection;
};
#endif

class k9HalConnection : public QObject
{
Q_OBJECT
friend class k9HalDevice;
public:
    static k9HalConnection* getInstance();
    static void end();

    QPtrList< k9HalDevice > getDevices() const { return m_devices;}
    void addDevice( const char* udi );
    void removeDevice( const char* udi );
    void testVolumeChanged( const char * udi);
    k9HalDevice *findDevice (const char* udi);
    k9HalDevice *findDeviceByVolume (const char* udi);
signals:
    void deviceAdded(k9HalDevice *);
    void deviceRemoved(k9HalDevice*);
private:
    QPtrList <k9HalDevice> m_devices;
    void *m_context;
    DBusConnection* m_dbusConnect;
    #ifdef DBUS_QT3
    QDBusConnection* m_dbusQtConnect;
    #else
    DBusQt::Connection* m_dBusQtConnect;
    #endif

private:

    k9HalConnection(QObject *parent = 0, const char *name = 0);

    ~k9HalConnection();

    
};

#endif
#endif
