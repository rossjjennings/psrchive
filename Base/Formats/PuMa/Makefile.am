
noinst_LTLIBRARIES = libpuma.la

nobase_include_HEADERS = Pulsar/PuMaArchive.h

libpuma_la_SOURCES = PuMaArchive.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = @PUMA_CFLAGS@

include $(top_srcdir)/config/Makefile.include

libpuma_la_LIBADD = @PUMA_LIBS@

