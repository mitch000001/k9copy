AM_CPPFLAGS= -I$(srcdir) $(all_includes) 

KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)

METASOURCES = AUTO



# lib version 2 for K3b 0.11.x 

noinst_HEADERS = k3bcdtext.h k3bdeviceglobals.h k3bdevice.h \
	k3bdevicemanager.h	k3bdevicetypes.h k3bdiskinfo.h k3bmsf.h k3btoc.h k3btrack.h k3bscsicommand_bsd.cpp \
	k3bscsicommand_linux.cpp

libk3bdevice_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libk3bdevice.la
libk3bdevice_la_SOURCES = k3bcdtext.cpp k3bcrc.cpp k3bdevice.cpp\
	k3bdeviceglobals.cpp k3bdevicemanager.cpp k3bdevice_mmc.cpp\
	k3bdiskinfo.cpp k3bmsf.cpp k3bscsicommand.cpp k3btoc.cpp k3btrack.cpp
