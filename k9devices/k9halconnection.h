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
#ifndef K9HALCONNECTION_H
#define K9HALCONNECTION_H

#include <qobject.h>
#include <qptrlist.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9HalDevice;
class DBusConnection;
namespace DBusQt {
  class Connection;
};

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
    k9HalDevice *findDevice (const char* udi);
signals:
    void deviceAdded(k9HalDevice *);
    void deviceRemoved(k9HalDevice*);
private:
    QPtrList <k9HalDevice> m_devices;
    void *m_context;
    DBusConnection* m_dbusConnect;
    DBusQt::Connection* m_dBusQtConnect;

private:

    k9HalConnection(QObject *parent = 0, const char *name = 0);

    ~k9HalConnection();

    
};

#endif
