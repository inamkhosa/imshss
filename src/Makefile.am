SUBDIRS= oamps_module cx_app db_module sh_app 

bin_PROGRAMS = hss

hss_SOURCES = hss.h hss.c 

INCLUDES = $(DIAMETER_DEPS_CFLAGS) $(DIAMAPPCOMMON_DEPS_CFLAGS) $(LIBXML_DEPS_CFLAGS) $(LIBAMPS_DEPS_CFLAGS)

LDADD = $(DIAMETER_DEPS_LIBS) $(DIAMAPPCOMMON_DEPS_LIBS) $(LIBXML_DEPS_LIBS) $(LIBAMPS_DEPS_LIBS)

AM_CFLAGS = -DDATADIR=\"$(datadir)/@PACKAGE@/\" -DLIBDIR=\"$(libdir)/\"

hssextradir = $(datadir)/@PACKAGE@
hssextra_DATA = $(top_srcdir)/config/*
