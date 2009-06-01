

h_sources = hss_oamps.h oamps_xmlAPI.h $(top_srcdir)/src/hss.h

c_sources = hss_oamps.c oamps_xmlAPI.c

INCLUDES = -I. -I$(top_srcdir)/src -I$(top_srcdir)/include/amps -I$(top_srcdir)/include/libxml2 $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) -L$(top_srcdir)/lib -lamps -lxml2

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libhss_oamps.la
libhss_oamps_la_SOURCES= $(h_sources) $(c_sources)
libhss_oamps_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
