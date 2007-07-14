# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/k9Mplayer -I$(top_srcdir)/k9decmpeg \
	-I$(top_srcdir)/k9devices -I$(top_srcdir)/k9vamps -I$(top_srcdir)/libdvdnav \
	-I$(top_srcdir)/libk9copy $(all_includes)

# these are the headers for your project
noinst_HEADERS = k9copy.h k9glwidget.h k9langselect.h k9main.h \
	k9mencodercmdgen.h k9mp4title.h k9play.h k9playbackoptions.h k9prefdvd.h k9prefmencoder.h \
	k9prefmpeg4.h k9prefpreview.h k9settings.h k9titlefactor.h k9updatefactor.h \
	kconfigdlg.h

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
k9copy_SOURCES = configDlg.ui images.h k9copy.cpp k9copy.png k9glwidget.cpp \
	k9langselect.cpp k9main.cpp k9mainw.ui k9mencodercmdgen.cpp k9mp4title.cpp k9play.cpp \
	k9playbackoptions.cpp k9prefdvd.cpp k9prefmencoder.cpp k9prefmpeg4.cpp k9prefpreview.cpp \
	k9settings.cpp k9titlefactor.cpp k9updatefactor.cpp kconfigdlg.cpp kviewmpeg2.cpp \
	kviewmpeg2.h langselectw.ui main.cpp mencoderCmdGen.ui playbackoptionsw.ui \
	prefDVD.ui prefMPEG4.ui prefMencoder.ui prefpreview.ui titlefactor.ui viewmpeg2.ui
k9copy_LDFLAGS = $(KDE_RPATH) $(all_libraries)
k9copy_LDADD = $(top_builddir)/k9Mplayer/libk9mplayer.la \
	$(top_builddir)/libdvdnav/libk9dvdnav.la $(top_builddir)/libk9copy/libk9copy.la \
	$(top_builddir)/k9decmpeg/libk9decmpeg.la $(top_builddir)/k9vamps/libk9vamps.la \
	$(top_builddir)/k9devices/libk9devices.la -lkmdi $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities


# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/k9copy
shellrc_DATA = k9copyui.rc

appsdatadir =$(kde_datadir)/k9copy
appsdata_DATA = anim.gif mencoder.xml

AM_CXXFLAGS = -O3 -g3

applnkMultimedia_DATA = k9copy.desktop
applnkMultimediadir = $(kde_appsdir)/Multimedia
SUBDIRS = icons

konquerorServicesMenusdir = $(kde_datadir)/konqueror/servicemenus

konquerorServicesMenus_DATA = k9copy_open.desktop
