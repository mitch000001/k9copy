AM_CPPFLAGS= -I$(srcdir) $(all_includes) 

KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)

METASOURCES = AUTO

noinst_LTLIBRARIES = libk9copy.la


libk9copy_la_SOURCES = backupdlg.ui dummy.cpp dvdprogress.ui k9audiocodecs.cpp \
	k9backupdlg.cpp k9burndvd.cpp k9cell.cpp k9cellcopylist.cpp k9config.cpp \
	k9drawimage.cpp k9dvd.cpp k9dvdauthor.cpp k9dvdbackup.cpp k9dvdchapter.cpp \
	k9dvdprogress.cpp k9dvdread.cpp k9dvdsize.cpp k9dvdtitle.cpp k9dvdtitleset.cpp k9ifo2.cpp \
	k9mp4dlg.cpp k9mp4enc.cpp k9process.cpp k9progress.cpp k9saveimage.cpp k9script.cpp \
	k9titleencopt.cpp k9tools.cpp k9videocodecs.cpp mp4dlg.ui progress.ui

include_HEADERS = 

noinst_HEADERS = bswap.h k9audiocodecs.h k9backupdlg.h k9burndvd.h k9cell.h \
	k9cellcopylist.h k9common.h k9config.h k9drawimage.h k9dvd.h k9dvdauthor.h k9dvdbackup.h \
	k9dvdchapter.h k9dvdprogress.h k9dvdread.h k9dvdsize.h k9dvdtitle.h k9dvdtitleset.h \
	k9ifo2.h k9mp4dlg.h k9mp4enc.h k9process.h k9progress.h k9saveimage.h k9script.h \
	k9titleencopt.h k9tools.h k9videocodecs.h


INCLUDES = -I$(top_srcdir)/k9decmpeg -I$(top_srcdir)/k9vamps \
	-I$(top_srcdir)/libdvdnav
libk9copy_la_LDFLAGS = $(all_libraries)
