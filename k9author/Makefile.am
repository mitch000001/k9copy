AM_CPPFLAGS= -I$(srcdir) $(all_includes) 

KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)

METASOURCES = AUTO

noinst_LTLIBRARIES = libk9author.la
libk9author_la_LDFLAGS = $(all_libraries)
libk9author_la_SOURCES = chapterEdit.ui import.ui k9avidecode.cpp k9avifile.cpp \
	k9canvasselection.cpp k9chapteredit.cpp k9import.cpp k9importfiles.cpp k9lvitemimport.cpp \
	k9menu.cpp k9menubutton.cpp k9menuedit.cpp k9menueditor.cpp k9newdvd.cpp \
	k9newtitle.cpp k9title.cpp menuEdit.ui newTitle.ui
noinst_HEADERS = k9avidecode.h k9avifile.h k9canvasselection.h k9chapteredit.h \
	k9import.h k9importfiles.h k9lvitemimport.h k9menu.h k9menubutton.h k9menuedit.h \
	k9menueditor.h k9newdvd.h k9newtitle.h k9title.h
libk9author_la_LIBADD = $(top_builddir)/libk9copy/libk9copy.la $(LIB_KDEUI)
INCLUDES = -I$(top_srcdir)/dvdread -I$(top_srcdir)/k9devices \
	-I$(top_srcdir)/k9vamps -I$(top_srcdir)/libk9copy -I$(top_srcdir)/src $(all_includes)
