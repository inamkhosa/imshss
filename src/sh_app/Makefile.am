

h_sources = sh_app.h  sh_avps.h sh_diamretcodes.h sh_msgvalidator.h sh_PUR.h sh_UDR.h sh_msg_api.h $(top_srcdir)/src/events.h

c_sources = sh_app.c sh_msgvalidator.c sh_PUR.c sh_UDR.c sh_msg_api.c

INCLUDES = -I. -I$(top_srcdir)/src -I$(top_srcdir)/src/oamps_module $(LIBAMPS_DEPS_CFLAGS) $(LIBXML_DEPS_CFLAGS) $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) $(LIBXML_DEPS_LIBS)

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libsh_app.la
libsh_app_la_SOURCES= $(h_sources) $(c_sources)
libsh_app_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
