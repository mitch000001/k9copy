# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/k9decmpeg -I$(top_srcdir)/k9vamps \
	-I$(top_srcdir)/libk3bdevice -I$(top_srcdir)/libk9copy $(all_includes)

# these are the headers for your project
noinst_HEADERS = kconfigdlg.h kburndvd.h k9copy.h k9main.h k9play.h \
			k9settings.h k9prefmpeg4.h k9prefdvd.h k9prefmencoder.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO


KDE_ICON = k9copy

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = k9copy

# the application source, library search path, and link libraries
k9copy_SOURCES = images.h kcddrive.cpp kcddrive.h kviewmpeg2.cpp kviewmpeg2.h \
						menupreview.ui viewmpeg2.ui k9copy.png main.cpp configDlg.ui kconfigdlg.cpp \
						kburndvd.cpp k9mainw.ui k9copy.cpp k9main.cpp k9play.cpp k9settings.cpp prefDVD.ui \
			prefMPEG4.ui k9prefmpeg4.cpp k9prefdvd.cpp prefMencoder.ui k9prefmencoder.cpp
k9copy_LDFLAGS = $(KDE_RPATH) $(all_libraries)
k9copy_LDADD = $(top_builddir)/libk9copy/libk9copy.la \
	$(top_builddir)/k9decmpeg/libk9decmpeg.la $(top_builddir)/libk3bdevice/libk3bdevice.la \
	$(top_builddir)/k9vamps/libk9vamps.la $(LIB_KPARTS) $(LIB_KDEUI) -ldvdread

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities


# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/k9copy
shellrc_DATA = k9copyui.rc


AM_CXXFLAGS = -O3 -g3

applnkMultimedia_DATA = k9copy.desktop
applnkMultimediadir = $(kde_appsdir)/Multimedia
SUBDIRS = icons
