//
// C++ Interface: k9haldevice
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9HALDEVICE_H
#define K9HALDEVICE_H

#include <qobject.h>
#include <qtimer.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9HalConnection;
class k9HalDevice : public QObject
{
Q_OBJECT
public:   
    
    k9HalDevice(QObject *parent = 0, const char *name = 0);

    ~k9HalDevice();

	bool getCanReadCd() const {
	    return m_canReadCd;
	}

	bool getCanReadDvd() const {
	    return m_canReadDvd;
	}

	bool getCanBurnCd() const {
	    return m_canBurnCd;
	}

	bool getCanBurnDvd() const {
	    return m_canBurnDvd;
	}

	QString getModel() const {
	    return m_model;
	}

	int getMaxWriteSpeed() const {
	    return m_maxWriteSpeed;
	}

	QString getDeviceName() const {
	    return m_deviceName;
	}
signals:
   void volumeChanged(const QString &device);	
private slots:
    void timerDone();
	
private:
    QTimer m_timer;
    QString m_volumeName;
    k9HalConnection *m_connection;
    bool m_canReadCd,m_canReadDvd,m_canBurnCd,m_canBurnDvd;
    QString m_model;
    QString m_deviceName;
    int m_maxWriteSpeed;
    void getDriveProperties();
    QString volumeName();
};

#endif
