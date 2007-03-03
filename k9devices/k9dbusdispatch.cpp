//
// C++ Implementation: k9dbusdispatch
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9dbusdispatch.h"
#ifdef HAVE_HAL
#include <dbus/dbus.h>

//==============================callbacks ========================================


static dbus_bool_t qDBusAddWatch(DBusWatch *watch, void *data)
{

    int flags = dbus_watch_get_flags(watch);
    int fd = dbus_watch_get_fd(watch);
    
    K9DBusDispatch *d=(K9DBusDispatch*)data;
    
    k9Watcher watcher;
    if (flags & DBUS_WATCH_READABLE) {
        bool enabled = dbus_watch_get_enabled(watch);
        //qDebug("addReadWatch %d %s", fd, (enabled ? "enabled" : "disabled"));
        watcher.watch = watch;
            watcher.read = new QSocketNotifier(fd, QSocketNotifier::Read, d);
            if (!enabled) watcher.read->setEnabled(false);
            d->connect(watcher.read, SIGNAL(activated(int)), SLOT(socketRead(int)));
    }
    if (flags & DBUS_WATCH_WRITABLE) {
        bool enabled = dbus_watch_get_enabled(watch);
        //qDebug("addWriteWatch %d %s", fd, (enabled ? "enabled" : "disabled"));
        watcher.watch = watch;
            watcher.write = new QSocketNotifier(fd, QSocketNotifier::Write, d);
            if (!enabled) watcher.write->setEnabled(false);
            d->connect(watcher.write, SIGNAL(activated(int)), SLOT(socketWrite(int)));
    }
    // FIXME-QT4 d->watchers.insertMulti(fd, watcher);
    K9DBusDispatch::WatcherHash::iterator it = d->watchers.find(fd);
    if (it == d->watchers.end())
    {
        it = d->watchers.insert(fd, K9DBusDispatch::WatcherList());
    }
    it.data().append(watcher);

    return true;
}

static void qDBusRemoveWatch(DBusWatch *watch, void *data)
{
    K9DBusDispatch *d = (K9DBusDispatch*)data;
    
    int fd = dbus_watch_get_fd(watch);

    K9DBusDispatch::WatcherHash::iterator it = d->watchers.find(fd);
    if (it != d->watchers.end())
    {
        K9DBusDispatch::WatcherList& list = *it;
        for (K9DBusDispatch::WatcherList::iterator wit = list.begin();
             wit != list.end(); ++wit)
        {
            if ((*wit).watch == watch)
            {
                // migth be called from a function triggered by a socket listener
                // so just disconnect them and schedule their delayed deletion.

                d->removedWatches.append(*wit);
                if ((*wit).read)
                {
                    (*wit).read->disconnect(d);
                    (*wit).read = 0;
                }
                if ((*wit).write)
                {
                    (*wit).write->disconnect(d);
                    (*wit).write = 0;
                }
                (*wit).watch = 0;
            }
        }
    }

    if (d->removedWatches.count() > 0)
        QTimer::singleShot(0, d, SLOT(purgeRemovedWatches()));

}

static void qDBusToggleWatch(DBusWatch *watch, void *data)
{
    K9DBusDispatch *d=(K9DBusDispatch*)data;
    int fd = dbus_watch_get_fd(watch);

    K9DBusDispatch::WatcherHash::iterator it = d->watchers.find(fd);
    if (it != d->watchers.end()) {
        K9DBusDispatch::WatcherList& list = *it;
        for (K9DBusDispatch::WatcherList::iterator wit = list.begin(); wit != list.end();
             ++wit)
        {
            if ((*wit).watch == watch) {
                bool enabled = dbus_watch_get_enabled(watch);
                int flags = dbus_watch_get_flags(watch);

//                 qDebug("toggle watch %d to %d (write: %d, read: %d)",
//                         dbus_watch_get_fd(watch), enabled,
//                         flags & DBUS_WATCH_WRITABLE, flags & DBUS_WATCH_READABLE);

                if (flags & DBUS_WATCH_READABLE && (*wit).read)
                    (*wit).read->setEnabled(enabled);
                if (flags & DBUS_WATCH_WRITABLE && (*wit).write)
                    (*wit).write->setEnabled(enabled);
                return;
            }
        }
    }
}


void K9DBusDispatch::purgeRemovedWatches()
{
    if (removedWatches.isEmpty()) return;

    WatcherList::iterator listIt = removedWatches.begin();
    for (; listIt != removedWatches.end(); ++listIt)
    {
        delete (*listIt).read;
        delete (*listIt).write;
    }
    removedWatches.clear();

    uint count = 0;
    WatcherHash::iterator it = watchers.begin();
    while (it != watchers.end())
    {
        WatcherList& list = *it;
        listIt = list.begin();
        while (listIt != list.end())
        {
            if (!((*listIt).read) && !((*listIt).write))
            {
                listIt = list.erase(listIt);
                ++count;
            }
        }

        if (list.isEmpty())
        {
            WatcherHash::iterator copyIt = it;
            ++it;
            watchers.erase(copyIt);
        }
        else
            ++it;
    }
}
//==============================================================================

void K9DBusDispatch::socketRead(int fd)
{
    // FIXME-QT4 QHashIterator<int, QDBusConnectionPrivate::Watcher> it(watchers);
    WatcherHash::const_iterator it = watchers.find(fd);
    if (it != watchers.end()) {
        const WatcherList& list = *it;
        for (WatcherList::const_iterator wit = list.begin(); wit != list.end(); ++wit) {
            if ((*wit).read && (*wit).read->isEnabled()) {
                if (!dbus_watch_handle((*wit).watch, DBUS_WATCH_READABLE))
                    qDebug("OUT OF MEM");
            }
        }
    }
    scheduleDispatch();
}

void K9DBusDispatch::socketWrite(int fd)
{
    // FIXME-QT4 QHashIterator<int, QDBusConnectionPrivate::Watcher> it(watchers);
    WatcherHash::const_iterator it = watchers.find(fd);
    if (it != watchers.end()) {
        const WatcherList& list = *it;
        for (WatcherList::const_iterator wit = list.begin(); wit != list.end(); ++wit) {
            if ((*wit).write && (*wit).write->isEnabled()) {
                if (!dbus_watch_handle((*wit).watch, DBUS_WATCH_WRITABLE))
                    qDebug("OUT OF MEM");
            }
        }
    }
}

void K9DBusDispatch::scheduleDispatch()
{
    m_dispatcher->start(0);
}

void K9DBusDispatch::dispatch()
{
        if (dbus_connection_dispatch(m_connection) != DBUS_DISPATCH_DATA_REMAINS)
        {
            // stop dispatch timer
            m_dispatcher->stop();
        }
}

K9DBusDispatch::K9DBusDispatch(QObject *parent, const char *name)
 : QObject(parent, name)
{
    m_dispatcher = new QTimer(this);
    QObject::connect(m_dispatcher, SIGNAL(timeout()), this, SLOT(dispatch()));
}


K9DBusDispatch::~K9DBusDispatch()
{
}


void K9DBusDispatch::setConnection(DBusConnection* _value) {
   m_connection = _value;

   dbus_connection_set_exit_on_disconnect(m_connection, false);
   dbus_connection_set_watch_functions(m_connection, qDBusAddWatch, qDBusRemoveWatch,
                                        qDBusToggleWatch, this, 0);
}

#include "k9dbusdispatch.moc"
#endif

