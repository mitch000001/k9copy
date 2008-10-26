#include "config.h"
#ifndef __STDC_LIMIT_MACROS
	#define __STDC_LIMIT_MACROS
#endif

#if defined(HAVE_INTTYPES_H)
    #include <inttypes.h>
#elif defined( HAVE_STDINT_H)
     #include <stdint.h>
#endif

#include "ifo_types.h"
#include "dvd_reader.h"
#include "ifo_read.h"
#include "nav_read.h"
#include "nav_types.h"
