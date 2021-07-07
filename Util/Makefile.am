
SUBDIRS                 = epsic/src units third genutil fft stat tempo

lib_LTLIBRARIES         = libpsrutil.la

libpsrutil_la_SOURCES  =
libpsrutil_la_LIBTOOLFLAGS = --tag=CXX
libpsrutil_la_LIBADD  = third/libthird.la units/libunits.la fft/libpsrfft.la \
			genutil/libgenutil.la stat/libstat.la \
			tempo/libtempo.la \
			epsic/src/libepsic.la @FLIBS@

if HAVE_CFITSIO
  SUBDIRS += fitsutil
  libpsrutil_la_LIBADD += fitsutil/libfitsutil.la
endif

if HAVE_TEMPO2
  SUBDIRS += tempo2
  libpsrutil_la_LIBADD += tempo2/libtempo_2.la
endif

if HAVE_QT
  SUBDIRS += qtutil
  libpsrutil_la_LIBADD += qtutil/libqtutil.la
endif

SUBDIRS += resources
libpsrutil_la_LIBADD += resources/libresources.la

include $(top_srcdir)/config/Makefile.local

NOW_IN_EPSIC := Basis.h Dirac.h Matrix.h Stokes.h Estimate.h complex_promote.h \
	BoxMuller.h Jacobi.h Minkowski.h Quaternion.h Traits.h random.h \
	Conventions.h Jones.h Pauli.h Spinor.h Vector.h PromoteTraits.h

all-local:
	-for file in $(NOW_IN_EPSIC); do \
	rm -f $(includedir)/$$file $(top_builddir)/local_include/$$file ; done

