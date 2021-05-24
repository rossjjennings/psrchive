
SUBDIRS = parsifal star gcvspl

noinst_LTLIBRARIES = libthird.la

libthird_la_SOURCES =

libthird_la_LIBADD = \
	parsifal/libparsifal.la \
	star/libstar.la \
	gcvspl/libgcvspl.la

if !HAVE_GETOPT_LONG
SUBDIRS += gnu
libthird_la_LIBADD += gnu/libgnu.la
endif

include $(top_srcdir)/config/Makefile.local

