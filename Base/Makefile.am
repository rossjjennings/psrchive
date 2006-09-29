
SUBDIRS = Bases Extensions Formats . Applications

lib_LTLIBRARIES = libpsrcore.la
libpsrcore_la_SOURCES  =
libpsrcore_la_LIBADD = \
	Bases/libBases.la \
	Extensions/libExtensions.la \
	Formats/libFormats.la

all-am: intermediate-install

include $(top_srcdir)/config/Makefile.local

