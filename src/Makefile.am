SUBDIRS= oamps_module cx_app db_module sh_app 

bin_PROGRAMS = hss

hss_SOURCES = hss.h hss.c 

INCLUDES = $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS) -I$(top_srcdir)/include/amps -I$(top_srcdir)/include/libxml2

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) -L/usr/local/lib -lamps -lxml2

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

hssextradir = $(datadir)/@PACKAGE@
hssextra_DATA = $(top_srcdir)/config/*.xml


