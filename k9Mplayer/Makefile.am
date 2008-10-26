INCLUDES = -I$(top_srcdir)/dvdread -I$(top_srcdir)/libk9copy $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libk9mplayer.la
libk9mplayer_la_SOURCES = mplayer.ui k9mplayer.cpp

libk9mplayer_la_LIBADD = $(top_builddir)/libdvdnav/libk9dvdnav.la
noinst_HEADERS = k9mplayer.h
