
lib_LTLIBRARIES = libFormats.la
libFormats_la_SOURCES  =

SUBDIRS = Timer EPN ASCII

libFormats_la_LIBADD = Timer/libtimer.la EPN/libepn.la ASCII/libascii.la

if HAVE_CFITSIO
SUBDIRS += PSRFITS 
libFormats_la_LIBADD += PSRFITS/libpsrfits.la
endif

all: intermediate-install

include $(top_srcdir)/config/Makefile.local

