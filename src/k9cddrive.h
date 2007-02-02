//
// C++ Interface: k9cddrive
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9CDDRIVE_H
#define K9CDDRIVE_H
#include "k9common.h"

#include <qobject.h>
#include "kconfigdlg.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>


/**
  *@author 
  */
class k9HalDevice;
class k9HalConnection;
class k9CdDrive : public QObject {
    Q_OBJECT
public:
    k9CdDrive();
    ~k9CdDrive();
    int num;
    QString name;
    QString device;
    bool canWriteCDR;
    bool canReadDVD;
    bool canWriteDVD;

    void setWriteSpeeds(const QValueList< int >& _value);
    QValueList< int > getWriteSpeeds() const;

    void setDevice(k9HalDevice* _value) { m_Device = _value;}
	
    k9HalDevice* getDevice()  { return m_Device;}
	
private:
   QValueList <int> writeSpeeds;
   k9HalDevice *m_Device;
};

class k9CdDrives : public QObject  {
    Q_OBJECT
public:
    k9CdDrives();
    ~k9CdDrives();
    /** No descriptions */
    int count();
    /** No descriptions */
    k9CdDrive * getDrive(int num);
    void eject(const QString & device);
    void scanDrives();
public slots:
   void deviceAdded(k9HalDevice *device);
   void deviceRemoved(k9HalDevice *device);
signals:
    void deviceAdded(k9CdDrive *drive);
    void deviceRemoved(k9CdDrive *drive);
private: // Private methods
    k9HalConnection *m_connection;
    void addDrive(k9HalDevice *_device);
    void readConfig();
    QPtrList <k9CdDrive> drives;
};
#endif
