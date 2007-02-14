//
// C++ Implementation: k9haldevice
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9haldevice.h"
#include "k9halconnection.h"
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/connection.h>
#include <dbus/dbus.h>
#include <hal/libhal.h>
#include <hal/libhal-storage.h>

k9HalDevice::k9HalDevice(QObject *parent, const char *udi)
        : QObject(parent, udi) {

    m_connection=(k9HalConnection*)parent;
    getDriveProperties();
}

k9HalDevice::~k9HalDevice() {}

QString k9HalDevice::volumeName() {
    QString sVol("");
    LibHalDrive *drive= libhal_drive_from_udi ((LibHalContext *)m_connection->m_context, name());
    int num_volumes;
    if (drive !=NULL) {
	char** volumes = libhal_drive_find_all_volumes ((LibHalContext *)m_connection->m_context, drive, &num_volumes);
	if (volumes != NULL)
		for (int i = 0; i < num_volumes; i++) {
		char *volume_udi;
		LibHalVolume *volume;
		volume_udi = volumes[i];
		QString s(volume_udi);
		m_volumeUdi=s;
		volume = libhal_volume_from_udi ((LibHalContext *)m_connection->m_context, volume_udi);
		if (volume != NULL) {
			sVol=QString(libhal_volume_get_label (volume));            
			libhal_volume_free (volume);
		}
	}
	libhal_free_string_array (volumes);
	libhal_drive_free(drive);
    }
    return sVol;
}

QString k9HalDevice::mountPoint() {
    QString sMountPoint("");
    LibHalDrive *drive= libhal_drive_from_udi ((LibHalContext *)m_connection->m_context, name());
    int num_volumes;
    char** volumes = libhal_drive_find_all_volumes ((LibHalContext *)m_connection->m_context, drive, &num_volumes);
    if (volumes != NULL)
        for (int i = 0; i < num_volumes; i++) {
            char *volume_udi;
            LibHalVolume *volume;
            volume_udi = volumes[i];
            volume = libhal_volume_from_udi ((LibHalContext *)m_connection->m_context, volume_udi);
            if (volume != NULL) {
		sMountPoint=QString(libhal_volume_get_mount_point (volume));            
            }
            libhal_volume_free (volume);

        }
    libhal_free_string_array (volumes);
    libhal_drive_free(drive);
    return sMountPoint;
}


void k9HalDevice::updateVolumeName() {
   m_volumeName=volumeName();
   emit volumeChanged(this->getDeviceName());
}

void k9HalDevice::getDriveProperties() {
    LibHalContext *context=(LibHalContext *)m_connection->m_context;
    LibHalDrive *drive= libhal_drive_from_udi (context, name());
    LibHalDriveCdromCaps caps;
    caps= libhal_drive_get_cdrom_caps (drive);
    m_canReadCd=m_canReadDvd=m_canBurnCd=m_canBurnDvd=false;
    m_canReadCd= (caps & LIBHAL_DRIVE_CDROM_CAPS_CDROM)==LIBHAL_DRIVE_CDROM_CAPS_CDROM;
    m_canBurnCd= (caps & LIBHAL_DRIVE_CDROM_CAPS_CDR)==LIBHAL_DRIVE_CDROM_CAPS_CDR;
    m_canReadDvd=(caps & LIBHAL_DRIVE_CDROM_CAPS_DVDROM)==LIBHAL_DRIVE_CDROM_CAPS_DVDROM;
    m_canBurnDvd=(caps & LIBHAL_DRIVE_CDROM_CAPS_DVDR)==LIBHAL_DRIVE_CDROM_CAPS_DVDR;
    m_model=QString(libhal_drive_get_model(drive));
//    qDebug(QString("canReadDvd:%1 canBurnDvd:%2 model:%3").arg(m_canReadDvd).arg(m_canBurnDvd).arg(m_model));


    libhal_drive_free(drive);
    if (libhal_device_property_exists( (LibHalContext*) m_connection->m_context, name(), "storage.cdrom.write_speed", 0 )) {
        m_maxWriteSpeed= libhal_device_get_property_int( (LibHalContext*)m_connection->m_context, name(), "storage.cdrom.write_speed", 0 );
    }
    char* dev = libhal_device_get_property_string( (LibHalContext*) m_connection->m_context, name(), "block.device", 0 );
    m_deviceName=QString ( dev );
    libhal_free_string( dev );

    m_volumeName=volumeName();

}
#include "k9haldevice.moc"
