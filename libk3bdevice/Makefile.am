AM_CPPFLAGS= -I$(srcdir) $(all_includes) 

KDE_CXXFLAGS = $(ENABLE_PERMISSIVE_FLAG)

METASOURCES = AUTO



# lib version 2 for K3b 0.11.x 
libk3bdevice_la_LDFLAGS = $(all_libraries) $(CAM_LIB)

libk3bdevice_la_SOURCES = k3bdevice.cpp k3bdevice_mmc.cpp k3bscsicommand.cpp \
	k3btrack.cpp k3btoc.cpp k3bdevicemanager.cpp k3bmsf.cpp k3bdiskinfo.cpp \
	k3bdeviceglobals.cpp k3bcrc.cpp k3bcdtext.cpp k3bscsicommand_bsd.cpp \
	k3bscsicommand_linux.cpp

noinst_HEADERS = k3bdevicemanager.h k3bdevice.h k3btoc.h k3btrack.h \
	k3bdeviceglobals.h k3bdiskinfo.h k3bcdtext.h k3bmsf.h k3bdevicetypes.h

noinst_LTLIBRARIES = libk3bdevice.la
