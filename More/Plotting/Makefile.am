
noinst_LTLIBRARIES = libPlotter.la

##############################################################################
#
# Pretty much everything is PGPLOT-specific code
#
if HAVE_PGPLOT

nobase_include_HEADERS = Pulsar/PlotterFactory.h \
	Pulsar/ProfilePlotter.h Pulsar/ProfilePlotterTI.h \
	Pulsar/SinglePlotter.h \
	Pulsar/PosAngPlotter.h Pulsar/PosAngPlotterTI.h

libPlotter_la_SOURCES = PlotterFactory.C \
	ProfilePlotter.C ProfilePlotterTI.C \
	SinglePlotter.C \
	PosAngPlotter.C PosAngPlotterTI.C

bin_PROGRAMS = psrplot

psrplot_SOURCES = psrplot.C

endif

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/utils/include -I$(top_builddir)/astro/include \
	-I$(top_srcdir)/psrchive @PGPLOT_CFLAGS@ @SLA_CFLAGS@

LDADD = libPlotter.la $(top_builddir)/psrchive/libpsrchive.la	\
	$(top_builddir)/astro/libpsrastro.la			\
	$(top_builddir)/utils/libpsrutils.la @PGPLOT_LIBS@
