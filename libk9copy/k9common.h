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

#if defined(HAVE_INTTYPES_H)
    #include <inttypes.h>
#elif defined( HAVE_STDINT_H)
     #include <stdint.h>
#endif


#ifdef __FreeBSD__
    #include <osreldate.h>
#endif
#include <dvdread/ifo_types.h>
#include <dvdread/dvd_reader.h>
#include <dvdread/ifo_read.h>
#include <dvdread/ifo_print.h>
#include <dvdread/nav_read.h>
#include <dvdread/nav_types.h>

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
