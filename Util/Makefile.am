
SUBDIRS                 = epsic meal units third genutil fft stat tempo

lib_LTLIBRARIES         = libpsrutil.la

libpsrutil_la_LIBTOOLFLAGS = --tag=CXX
libpsrutil_la_SOURCES =
libpsrutil_la_LIBADD = third/libthird.la units/libunits.la fft/libpsrfft.la \
			genutil/libgenutil.la stat/libstat.la \
			tempo/libtempo.la \
			epsic/src/util/libutil.la epsic/src/true_math/libtrue_math.la \
			meal/src/util/libutil.la @FLIBS@

if HAVE_CFITSIO
  SUBDIRS += fitsutil
  libpsrutil_la_LIBADD += fitsutil/libfitsutil.la
endif

if HAVE_TEMPO2
  SUBDIRS += tempo2
  libpsrutil_la_LIBADD += tempo2/libtempo_2.la
endif

SUBDIRS += resources
libpsrutil_la_LIBADD += resources/libresources.la

include $(top_srcdir)/config/Makefile.local

