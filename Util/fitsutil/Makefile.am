
noinst_LTLIBRARIES = libfitsutil.la

include_HEADERS = fitsutil.h FITSError.h

libfitsutil_la_SOURCES = FITSError.C fitsutil.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_srcdir)/utils/units @CFITSIO_CFLAGS@

