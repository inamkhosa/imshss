

h_sources = cxdx_db.h db_cxdx_mar.h dbmodule.h db_sh_udr.h cxdx_userprofile.h db_cxdx_sar.h db_sh_pur.h db_cxdx_lir.h db_cxdx_uar.h db_sh_querycodes.h $(top_srcdir)/src/events.h 

c_sources = cxdx_db.c db_cxdx_lir.c db_cxdx_sar.c dbmodule.c db_sh_udr.c cxdx_userprofile.c db_cxdx_mar.c db_cxdx_uar.c db_sh_pur.c

INCLUDES = -I. -I$(top_srcdir)/src -I$(top_srcdir)/src/sh_app -I$(top_srcdir)/src/oamps_module $(LIBAMPS_DEPS_CFLAGS) $(LIBXML_DEPS_CFLAGS) $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) $(LIBXML_DEPS_LIBS)

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

lib_LTLIBRARIES= libhss_dbmodule.la
libhss_dbmodule_la_SOURCES= $(h_sources) $(c_sources)
libhss_dbmodule_la_LDFLAGS= -version-info $(LIBRARY_VERSION) -release $(RELEASE)
