
SUBDIRS                 = units genutil genutil++ psrfft

lib_LTLIBRARIES         = libpsrutils.la
libpsrutils_la_SOURCES  =
libpsrutils_la_LIBADD   = units/libunits.la psrfft/libpsrfft.la \
			genutil/libgenutil.la genutil++/libgenutilpp.la

if HAVE_CFITSIO
  SUBDIRS += fitsutil
  libpsrutils_la_LIBADD += fitsutil/libfitsutil.la
endif

if HAVE_MPI
  SUBDIRS += mpiutil
  libpsrutils_la_LIBADD += mpiutil/libmpiutil.la
endif

if HAVE_PGPLOT
  SUBDIRS += pgutil plot2d
  libpsrutils_la_LIBADD += pgutil/libpgutil.la plot2d/libplot2d.la
endif

if HAVE_QT
  SUBDIRS += qtutil
  libpsrutils_la_LIBADD += qtutil/libqtutil.la
endif


all: intermediate-install

include $(top_srcdir)/config/Makefile.local

