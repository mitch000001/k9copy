/*
 *
 * $Id: k3bdiskinfo.h 421129 2005-06-02 09:33:12Z trueg $
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2004 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#ifndef _K3B_DISKINFO_H_
#define _K3B_DISKINFO_H_

#include "k3bdevicetypes.h"

#include "k3btoc.h"
#include "k3bmsf.h"


#include <qstring.h>


class kdbgstream;


namespace K3bDevice
{
  /**
   * This class is directly accociated to a strcuture from 
   * the MMC draft READ_DISK_INFO.
   * It also holds some additional data.
   * This class' data will be retrieved by K3bDevice::Device.
   *
   * Before using any values one should check diskState != STATE_UNKNOWN or
   * diskState == STATE_NO_MEDIA.
   * That may mean that no disk is in the drive or an error occurred.
   * Writers should never give the STATE_UNKNOWN state. CD-ROM or DVD-ROM
   * drives on the other hand may have trouble determining the state of the disk.
   */
  class DiskInfo
    {
    public:
      DiskInfo();
      ~DiskInfo();

      /**
       * Returnes the state of the disk.
       * See enum State.
       */
      int diskState() const;

      /**
       * Returnes the state of the last session.
       * See enum State.
       */
      int lastSessionState() const;

      /**
       * Returnes the state of the background formatting. This does
       * only make sense for DVD+RW (and MRW which is not yet supported)
       */
      int bgFormatState() const;

      /**
       * returnes true if diskState() == STATE_EMPTY
       */
      bool empty() const;

      /**
       * Is this a rewritable media (e.g. a CD-RW, DVD-RW, or DVD+RW)
       */
      bool rewritable() const;

      /**
       * Is this disk appendable
       * returnes true if diskState() == STATE_INCOMPLETE
       */
      bool appendable() const;

      /**
       * The type of the disk:
       * CD-ROM, CD-R, CD-RW, DVD-ROM, DVD-R(W), DVD+R(W)
       */
      int mediaType() const;

      /**
       * This is the current profile of the drive. That means it may differ
       * from drive to drive.
       * -1 means no info.
       * Mainly interesting for the distiction of DVD-R(W) modes:
       * Sequential and Restricted Overwrite.
       */
      int currentProfile() const { return m_currentProfile; }

      /**
       * Just for easy implementation since there are so many
       * different DVD formats.
       */
      bool isDvdMedia() const;

      /**
       * The number of sessions on the disk.
       * This does not include any leadout or the last empty session
       * on a DVD+-R(W)
       */
      int numSessions() const;

      /**
       * The number of finished tracks.
       * This does not include the empty track.
       */
      int numTracks() const;

      /**
       * Number of layers on a DVD media. For CD media this is always 1.
       */
      int numLayers() const;

      /**
       * Does only make sense for appendable disks.
       */
      K3b::Msf remainingSize() const;

      /**
       * The capacity of the disk.
       * For complete disks this may be the same as size()
       */
      K3b::Msf capacity() const;

      /**
       * Returns the size of the used part.
       * For appendable media this equals capacity() - remainingSize()
       */
      K3b::Msf size() const;

      /**
       * Returns the size of Data area in the first layer for DL DVD media.
       * Otherwise size() is returned.
       *
       * This does not specify the layer change sector as the data area on DVD media does
       * not start at sector 0 but at sector 30000h or 31000h depending on the type.
       */
      K3b::Msf firstLayerSize() const;

      void debug() const;

    private:
      int m_mediaType;
      int m_currentProfile;

      int m_diskState;
      int m_lastSessionState;
      int m_bgFormatState;
      int m_numSessions;
      int m_numTracks;
      int m_numLayers;  // only for DVD media
      int m_rewritable;

      K3b::Msf m_capacity;
      K3b::Msf m_usedCapacity;
      K3b::Msf m_firstLayerSize;

      friend class Device;
    };

  //  kdbgstream& operator<<( kdbgstream& s, const DiskInfo& ngInf );
}


#endif
