
SUBDIRS = parsifal taygeta

noinst_LTLIBRARIES = libthird.la
libthird_la_SOURCES =
libthird_la_LIBADD = parsifal/libparsifal.la taygeta/libtaygeta.la

if !HAVE_SLA

SUBDIRS += star
libthird_la_LIBADD += star/libstar.la

endif

include $(top_srcdir)/config/Makefile.local

