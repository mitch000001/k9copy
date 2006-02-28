/* 
 *
 * $Id: k3btrack.cpp 330821 2004-07-19 09:15:18Z trueg $
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


#include "k3btrack.h"


K3bDevice::Track::Track()
  : m_type(-1),
    m_mode(-1),
    m_copyPermitted(true),
    m_preEmphasis(false),
    m_session(0)
{
}


K3bDevice::Track::Track( const Track& track )
  : m_firstSector( track.firstSector() ),
    m_lastSector( track.lastSector() ),
    m_index0( track.index0() ),
    m_type( track.type() ),
    m_mode( track.mode() ),
    m_copyPermitted( track.copyPermitted() ),
    m_preEmphasis( track.preEmphasis() ),
    m_session( track.session() ),
    m_indices( track.indices() )
{
}


K3bDevice::Track::Track( const K3b::Msf& firstSector, 
			   const K3b::Msf& lastSector, 
			   int type, 
			   int mode )
  : m_firstSector( firstSector ), 
    m_lastSector( lastSector ), 
    m_type( type ), 
    m_mode( mode ), 
    m_copyPermitted(true),
    m_preEmphasis(false),
    m_session(0)
{
}


K3bDevice::Track& K3bDevice::Track::operator=( const K3bTrack& track )
{
  if( this != &track ) {
    m_firstSector = track.firstSector();
    m_lastSector = track.lastSector();
    m_index0 = track.index0();
    m_type = track.type();
    m_mode = track.mode();
    m_indices = track.indices();
  }

  return *this;
}


K3b::Msf K3bDevice::Track::length() const
{
  // +1 since the last sector is included
  return m_lastSector - m_firstSector + 1;
}


K3b::Msf K3bDevice::Track::realAudioLength() const
{
  if( index0() > 0 )
    return index0();
  else
    return length();
}


void K3bDevice::Track::setIndex0( const K3b::Msf& msf )
{
  if( msf <= m_lastSector-m_firstSector )
    m_index0 = msf;
}


int K3bDevice::Track::indexCount() const
{
  return m_indices.count()-1;
}
