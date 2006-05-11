/*
 *
 * $Id: k3bscsicommand_linux.cpp 527315 2006-04-07 17:20:25Z trueg $
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2004 Sebastian Trueg <trueg@k3b.org>
 *
 * Parts of this file are inspired (and copied) from transport.hxx
 * from the dvd+rw-tools (C) Andy Polyakov <appro@fy.chalmers.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "k3bscsicommand.h"
#include "k3bdevice.h"

#include <kdebug.h>

#include <sys/ioctl.h>
#undef __STRICT_ANSI__
#include <linux/cdrom.h>
#define __STRICT_ANSI__
#include <scsi/sg.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>


#if !defined(SG_FLAG_LUN_INHIBIT)
# if defined(SG_FLAG_UNUSED_LUN_INHIBIT)
#  define SG_FLAG_LUN_INHIBIT SG_FLAG_UNUSED_LUN_INHIBIT
# else
#  define SG_FLAG_LUN_INHIBIT 0
# endif
#endif

#ifdef SG_IO
static bool useSgIo()
{
  struct utsname buf;
  uname( &buf );
  // was CDROM_SEND_PACKET declared dead in 2.5?
  return ( strcmp( buf.release, "2.5.43" ) >=0 );
}
#endif


class K3bDevice::ScsiCommand::Private
{
public:
  struct cdrom_generic_command cmd;
  struct request_sense sense;

#ifdef SG_IO
  bool useSgIo;
  struct sg_io_hdr sgIo;
#endif
};


void K3bDevice::ScsiCommand::clear()
{
  ::memset( &d->cmd, 0, sizeof(struct cdrom_generic_command) );
  ::memset( &d->sense, 0, sizeof(struct request_sense) );

  d->cmd.quiet = 1;
  d->cmd.sense = &d->sense;

#ifdef SG_IO
  d->useSgIo = useSgIo();
  ::memset( &d->sgIo, 0, sizeof(struct sg_io_hdr) );
#endif
}


unsigned char& K3bDevice::ScsiCommand::operator[]( size_t i )
{
#ifdef SG_IO
  if( d->sgIo.cmd_len < i+1 )
    d->sgIo.cmd_len = i+1;
#endif
  return d->cmd.cmd[i];
}


int K3bDevice::ScsiCommand::transport( TransportDirection dir,
				       void* data,
				       size_t len )
{
  bool needToClose = false;
  if( m_device ) {
    if( !m_device->isOpen() ) {
      needToClose = true;
    }
    m_device->open( dir == TR_DIR_WRITE );
    m_deviceHandle = m_device->handle();
  }

  if( m_deviceHandle == -1 )
    return -1;

  int i = -1;

#ifdef SG_IO
  if( d->useSgIo ) {
    d->sgIo.interface_id= 'S';
    d->sgIo.mx_sb_len = sizeof( struct request_sense );
    d->sgIo.cmdp      = d->cmd.cmd;
    d->sgIo.sbp       = (unsigned char*)&d->sense;
    d->sgIo.flags     = SG_FLAG_LUN_INHIBIT|SG_FLAG_DIRECT_IO;
    d->sgIo.dxferp    = data;
    d->sgIo.dxfer_len = len;
    if( dir == TR_DIR_READ )
      d->sgIo.dxfer_direction = SG_DXFER_FROM_DEV;
    else if( dir == TR_DIR_WRITE )
      d->sgIo.dxfer_direction = SG_DXFER_TO_DEV;
    else
      d->sgIo.dxfer_direction = SG_DXFER_NONE;

    i = ioctl( m_deviceHandle, SG_IO, &d->sgIo );

    if( ( d->sgIo.info&SG_INFO_OK_MASK ) != SG_INFO_OK )
      i = -1;
  }
  else {
#endif
    d->cmd.buffer = (unsigned char*)data;
    d->cmd.buflen = len;
    if( dir == TR_DIR_READ )
      d->cmd.data_direction = CGC_DATA_READ;
    else if( dir == TR_DIR_WRITE )
      d->cmd.data_direction = CGC_DATA_WRITE;
    else
      d->cmd.data_direction = CGC_DATA_NONE;
    
    i = ::ioctl( m_deviceHandle, CDROM_SEND_PACKET, &d->cmd );
#ifdef SG_IO
  }
#endif    

  if( needToClose )
    m_device->close();
    
  if( i ) {
    debugError( d->cmd.cmd[0],
		d->sense.error_code,
		d->sense.sense_key,
		d->sense.asc,
		d->sense.ascq );

    int errCode = 
      (d->sense.error_code<<24) & 0xF000 |
      (d->sense.sense_key<<16)  & 0x0F00 |
      (d->sense.asc<<8)         & 0x00F0 |
      (d->sense.ascq)           & 0x000F;

    return( errCode != 0 ? errCode : 1 );
  }
  else
    return 0;
}
