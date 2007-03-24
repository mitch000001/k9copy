AM_CPPFLAGS= -I$(srcdir) $(all_includes) 

KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)

METASOURCES = AUTO
libk9vamps_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libk9vamps.la
libk9vamps_la_SOURCES = getvlc.h k9vamps.cpp k9vamps.h putvlc.h qTable.h \
	k9requant.cpp cputest.cpp tcmemcpy.cpp k9fifo.cpp
INCLUDES = -I$(top_srcdir)/libk9copy
noinst_HEADERS = k9requant.h ac.h k9fifo.h
