## Process this file with automake to produce Makefile.in

if HAVE_PGPLOT
  PGPLOT_SUBDIRS = pgutil plot2d
  PGPLOT_LIBADD = pgutil/libpgutil.la plot2d/libplot2d.la
endif

if HAVE_QT
  QT_SUBDIRS = qtutil
  QT_LIBADD = qtutil/libqtutil.la
endif


SUBDIRS                 = units genutil genutil++ psrfft \
			$(PGPLOT_SUBDIRS) $(QT_SUBDIRS) .


lib_LTLIBRARIES         = libpsrutils.la
libpsrutils_la_SOURCES  =
libpsrutils_la_LIBADD   = units/libunits.la psrfft/libpsrfft.la \
			genutil/libgenutil.la genutil++/libgenutilpp.la \
			$(PGPLOT_LIBADD) $(QT_LIBADD)


# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libpsrutils_la_SOURCES = dummy.C
