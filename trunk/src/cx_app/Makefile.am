

h_sources = cxdx_app.h cxdx_mar.h cxdx_msgvalidation.h cxdx_uar.h cxdx_auth.h cxdx_msg_api.h cxdx_sar.h cxdx_lir.h cxdx_msgextractor.h cxdx_session.h $(top_srcdir)/src/events.h $(top_srcdir)/src/db_cmd_id.h $(top_srcdir)/src/hss.h $(top_srcdir)/src/oamps_module/hss_oamps.h

c_sources = cxdx_app.c cxdx_mar.c cxdx_msgvalidation.c cxdx_uar.c cxdx_auth.c cxdx_msg_api.c cxdx_sar.c cxdx_lir.c cxdx_msgextractor.c cxdx_session.c

INCLUDES = -I. -I$(top_srcdir)/src -I$(top_srcdir)/src/oamps_module $(LIBAMPS_DEPS_CFLAGS) $(top_srcdir)/src $(LIBXML_DEPS_CFLAGS) $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) $(LIBXML_DEPS_LIBS)

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libcxdx_app.la
libcxdx_app_la_SOURCES= $(h_sources) $(c_sources)
libcxdx_app_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
