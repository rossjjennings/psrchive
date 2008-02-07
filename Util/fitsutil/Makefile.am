
noinst_LTLIBRARIES = libfitsutil.la

include_HEADERS = fitsutil.h FITSError.h psrfitsio.h

libfitsutil_la_SOURCES = FITSError.C fitsutil.C psrfitsio.C

#############################################################################
#

libfitsutil_la_LIBADD = @CFITSIO_LIBS@

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @CFITSIO_CFLAGS@

