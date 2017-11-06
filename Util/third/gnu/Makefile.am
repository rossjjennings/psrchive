
noinst_LTLIBRARIES = libgnu.la

include_HEADERS =
libgnu_la_SOURCES =

if !HAVE_GETOPT_LONG
libgnu_la_SOURCES += getopt.c getopt1.c
include_HEADERS += getopt.h
endif

include $(top_srcdir)/config/Makefile.include

