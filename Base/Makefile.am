
lib_LTLIBRARIES = libFile.la
libFile_la_SOURCES  =

SUBDIRS = Base Extension Timer EPN ASCII

libFile_la_LIBADD = Base/libBase.la Extension/libExtension.la \
			Timer/libtimer.la EPN/libepn.la ASCII/libascii.la

if HAVE_CFITSIO
SUBDIRS += PSRFITS 
libFile_la_LIBADD += PSRFITS/libpsrfits.la
endif

SUBDIRS += . Application

include $(top_srcdir)/config/Makefile.local

