## Process this file with automake to produce Makefile.in

if HAVE_PGPLOT
  PGPLOT_SUBDIRS = pgutil plot2d
  PGPLOT_LIBADD = pgutil/libpgutil.la plot2d/libplot2d.la
endif

if HAVE_QT
  QT_SUBDIRS = qtutil
  QT_LIBADD = qtutil/libqtutil.la
endif

if HAVE_CFITSIO
  CFITSIO_SUBDIRS = fitsutil
  CFITSIO_LIBAD = fitsutil/libfitsutil.la
endif


SUBDIRS                 = units genutil genutil++ psrfft \
			$(PGPLOT_SUBDIRS) $(QT_SUBDIRS) $(CFITSIO_SUBDIRS) .


lib_LTLIBRARIES         = libpsrutils.la
libpsrutils_la_SOURCES  =
libpsrutils_la_LIBADD   = units/libunits.la psrfft/libpsrfft.la \
			genutil/libgenutil.la genutil++/libgenutilpp.la \
			$(PGPLOT_LIBADD) $(QT_LIBADD) $(CFITSIO_LIBADD)

all: intermediate-install

intermediate-install:
	CPPROG="cp -p" $(MAKE) prefix=.. install-data

