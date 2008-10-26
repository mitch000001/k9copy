AM_CPPFLAGS= -I$(srcdir) $(all_includes) 

KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)

METASOURCES = AUTO

libk9decmpeg_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libk9decmpeg.la
libk9decmpeg_la_SOURCES = alloc.cpp attributes.h bswap.h convert_internal.h \
	cpu_accel.cpp cpu_state.cpp decode.cpp header.cpp idct_alpha.cpp idct_altivec.cpp \
	idct.cpp idct_mmx.cpp kdecmpeg2.cpp kdecmpeg2.h mmx.h motion_comp_alpha.cpp \
	motion_comp_altivec.cpp motion_comp.cpp motion_comp_mmx.cpp motion_comp_vis.cpp mpeg2convert.h \
	mpeg2.h mpeg2_internal.h rgb.cpp rgb_mmx.cpp rgb_vis.cpp slice.cpp tendra.h \
	uyvy.cpp video_out.h vis.h vlc.h k9decodethread.cpp k9plaympeg2.cpp
INCLUDES = -I$(top_srcdir)/dvdread -I$(top_srcdir)/k9vamps \
	-I$(top_srcdir)/libdvdnav -I$(top_srcdir)/libk9copy

noinst_HEADERS = k9decodethread.h k9plaympeg2.h

