
noinst_LTLIBRARIES = libSimulation.la

nobase_include_HEADERS = \
        Pulsar/SimPolnTiming.h \
        Pulsar/SyntheticPolnProfile.h \
	Pulsar/SimplePolnProfile.h

libSimulation_la_SOURCES = \
	SimPolnTiming.C \
	SyntheticPolnProfile.C \
	SimplePolnProfile.C

bin_PROGRAMS = pts psrsim

pts_SOURCES = pts.C
psrsim_SOURCES = psrsim.C

pts_LDADD = $(LDPLOT)

if HAVE_PGPLOT
LDPLOT = $(top_builddir)/More/Plotting/libpsrplot.la $(LDADD) @PGPLOT_LIBS@
else
LDPLOT = $(LDADD)
endif

#############################################################################
#

LDADD = libSimulation.la \
	$(top_builddir)/More/libpsrmore.la \
        $(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la \
        $(top_builddir)/Util/epsic/src/libepsic.la 


include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @GSL_CFLAGS@ @PSRXML_CFLAGS@ @CULA_CFLAGS@

