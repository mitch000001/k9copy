INCLUDES = -I$(top_srcdir)/libk9copy $(all_includes)
METASOURCES = AUTO
libk9mplayer_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
noinst_LTLIBRARIES = libk9mplayer.la
libk9mplayer_la_SOURCES = mplayer.ui factory.cpp k9mplayer.cpp

libk9mplayer_la_LIBADD = $(top_builddir)/libdvdnav/libk9dvdnav.la $(LIB_KDEUI)
noinst_HEADERS = k9mplayer.h
