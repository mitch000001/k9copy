#include "config.h"
#ifndef __STDC_LIMIT_MACROS
	#define __STDC_LIMIT_MACROS
#endif

#if defined(HAVE_INTTYPES_H)
    #include <inttypes.h>
#elif defined( HAVE_STDINT_H)
     #include <stdint.h>
#endif

#include <dvdread/ifo_types.h>
#include <dvdread/dvd_reader.h>
#include <dvdread/ifo_read.h>
#include <dvdread/ifo_print.h>
#include <dvdread/nav_read.h>
#include <dvdread/nav_types.h>
