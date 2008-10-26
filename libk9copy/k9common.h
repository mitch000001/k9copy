//
// C++ Interface: k9common
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"
#ifndef __STDC_LIMIT_MACROS
	#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif

#if defined(HAVE_INTTYPES_H)
    #include <inttypes.h>
#elif defined( HAVE_STDINT_H)
     #include <stdint.h>
#endif


#ifdef __FreeBSD__
    #include <osreldate.h>
#endif
#include "ifo_types.h"
#include "dvd_reader.h"
#include "ifo_read.h"
#include "nav_read.h"
#include "nav_types.h"

#include <qobject.h>
#include <qobjectlist.h>
#include <qptrlist.h>

#ifndef DVD_BLOCK_LEN
#define DVD_BLOCK_LEN 2048
#endif

#include "k9dvdsize.h"
#ifndef NDEBUG
	#define NDEBUG
#endif
