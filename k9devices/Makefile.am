INCLUDES = -I$(top_srcdir)/dvdread -I$(top_srcdir)/libk9copy $(DBUS_INCS) \
	$(HAL_INCS) $(K3B_INCS) $(all_includes)
METASOURCES = AUTO
libk9devices_la_LDFLAGS = $(all_libraries) 
noinst_LTLIBRARIES = libk9devices.la
noinst_HEADERS = k9halconnection.h k9haldevice.h k9cddrive.h k9dbusdispatch.h
libk9devices_la_SOURCES = k9halconnection.cpp k9haldevice.cpp k9cddrive.cpp \
	k9dbusdispatch.cpp

libk9devices_la_LIBADD = $(K3B_LIBS) $(HAL_DBUS_LIBS)
