INCLUDES = -I$(top_srcdir)/libk9copy $(all_includes) $(HAL_INCS) $(DBUS_INCS) $(DBUSQT_INCS)
METASOURCES = AUTO
libk9devices_la_LDFLAGS = $(all_libraries) 
noinst_LTLIBRARIES = libk9devices.la
noinst_HEADERS = k9halconnection.h k9haldevice.h
libk9devices_la_SOURCES = k9halconnection.cpp k9haldevice.cpp

libk9devices_la_LIBADD = $(HAL_DBUS_LIBS)
