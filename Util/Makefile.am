
SUBDIRS                 = units genutil fft

lib_LTLIBRARIES         = libpsrutils.la
libpsrutils_la_SOURCES  =
libpsrutils_la_LIBADD   = units/libunits.la fft/libpsrfft.la \
			genutil/libgenutil.la tempo/libtempo.la @FLIBS@

if HAVE_CFITSIO
  SUBDIRS += fitsutil
  libpsrutils_la_LIBADD += fitsutil/libfitsutil.la
endif

SUBDIRS += tempo

if HAVE_PGPLOT
  SUBDIRS += pgutil
  libpsrutils_la_LIBADD += pgutil/libpgutil.la
endif

if HAVE_QT
  SUBDIRS += qtutil
  libpsrutils_la_LIBADD += qtutil/libqtutil.la
endif

all: intermediate-install

include $(top_srcdir)/config/Makefile.local

