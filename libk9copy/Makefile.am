AM_CPPFLAGS= -I$(srcdir) $(all_includes) 

KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)

METASOURCES = AUTO

lib_LTLIBRARIES = libk9copy.la

libk9copy_la_LIBADD = $(top_builddir)/k9vamps/libk9vamps.la \
	$(top_builddir)/k9decmpeg/libk9decmpeg.la $(top_builddir)/libdvdnav/libk9dvdnav.la
libk9copy_la_SOURCES = backupdlg.ui dvdprogress.ui progress.ui mp4dlg.ui \
			k9backupdlg.cpp k9cellcopylist.cpp k9cell.cpp k9dvdauthor.cpp k9dvdbackup.cpp k9dvd.cpp \
			k9dvdprogress.cpp k9ifo.cpp k9dvdtitle.cpp dummy.cpp k9dvdsize.cpp k9dvdread.cpp \
			k9progress.cpp k9mp4enc.cpp k9mp4dlg.cpp k9burndvd.cpp k9tools.cpp

include_HEADERS = k9backupdlg.h k9cellcopylist.h k9cell.h k9dvdauthor.h \
			k9dvdbackup.h k9dvdprogress.h k9ifo.h k9dvd.h k9dvdtitle.h k9dvdsize.h k9common.h

noinst_HEADERS = bswap.h k9dvdread.h k9progress.h k9mp4enc.h k9mp4dlg.h \
		k9burndvd.h k9tools.h


libk9copy_la_LDFLAGS = -module
INCLUDES = -I$(top_srcdir)/k9decmpeg -I$(top_srcdir)/k9vamps \
	-I$(top_srcdir)/libdvdnav
