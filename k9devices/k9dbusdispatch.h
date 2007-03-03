//
// C++ Interface: k9dbusdispatch
//
// Description: the main goal of this class is to dispatch dbus messages so that libhal can
//              detect changes on devices.
//              This code is based on QDBusConnectionPrivate from the new Qt DBus bindings
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DBUSDISPATCH_H
#define K9DBUSDISPATCH_H
#include "k9common.h"

#ifdef HAVE_HAL

#include <qobject.h>
#include <qtimer.h>
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <qsocketnotifier.h>
#include <qvaluelist.h>
#include <qmap.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9Watcher {
public:
    k9Watcher() { read=write=0; watch=0;};
    DBusWatch *watch;
    QSocketNotifier *read,*write;
};

class K9DBusDispatch : public QObject
{
Q_OBJECT
public:
    K9DBusDispatch(QObject *parent = 0, const char *name = 0);

    ~K9DBusDispatch();
    void setConnection(DBusConnection* _value);
    typedef QValueList<k9Watcher> WatcherList;
    WatcherList removedWatches;
    typedef QMap<int, WatcherList> WatcherHash;
    WatcherHash watchers;    

private:
    QTimer *m_dispatcher;
    DBusConnection *m_connection;

    void purgeRemovedWatches();
    void scheduleDispatch();
private slots:
    void socketRead(int fd);
    void socketWrite(int fd);
    void dispatch();
    

};
#endif
#endif
