## Process this file with automake to produce Makefile.in

if HAVE_PGPLOT
  PGPLOT_SUBDIRS = pgutil plot2d
  PGPLOT_LIBADD = pgutil/libpgutil.la plot2d/libplot2d.la
endif

SUBDIRS                 = units genutil genutil++ $(PGPLOT_SUBDIRS) .

lib_LTLIBRARIES         = libpsrutils.la
libpsrutils_la_SOURCES  =
libpsrutils_la_LIBADD   = units/libunits.la genutil/libgenutil.la \
			genutil++/libgenutilpp.la $(PGPLOT_LIBADD)


