
noinst_LTLIBRARIES = libasp.la

include_HEADERS = Pulsar/ASPArchive.h

libasp_la_SOURCES = ASPArchive.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = @CFITSIO_CFLAGS@

include $(top_srcdir)/config/Makefile.include

libasp_la_LIBADD = @CFITSIO_LIBS@

