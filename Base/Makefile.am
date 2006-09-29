
SUBDIRS = Bases Extensions Formats . Applications

lib_LTLIBRARIES = libCore.la
libCore_la_SOURCES  =
libCore_la_LIBADD = \
	Bases/libBases.la \
	Extensions/libExtensions.la \
	Formats/libFormats.la

all-am: intermediate-install

include $(top_srcdir)/config/Makefile.local

