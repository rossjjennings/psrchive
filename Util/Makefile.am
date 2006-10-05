
SUBDIRS                 = units third genutil fft

lib_LTLIBRARIES         = libpsrutil.la
libpsrutil_la_SOURCES  =
libpsrutil_la_LIBADD   = third/libthird.la units/libunits.la fft/libpsrfft.la \
			genutil/libgenutil.la tempo/libtempo.la @FLIBS@

if HAVE_CFITSIO
  SUBDIRS += fitsutil
  libpsrutil_la_LIBADD += fitsutil/libfitsutil.la
endif

SUBDIRS += tempo

if HAVE_PGPLOT
  SUBDIRS += pgutil
  libpsrutil_la_LIBADD += pgutil/libpgutil.la
endif

if HAVE_QT
  SUBDIRS += qtutil
  libpsrutil_la_LIBADD += qtutil/libqtutil.la
endif

include $(top_srcdir)/config/Makefile.local

