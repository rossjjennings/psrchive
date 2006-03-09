
noinst_LTLIBRARIES = libPlotter.la

##############################################################################
#
# Pretty much everything is PGPLOT-specific code
#
if HAVE_PGPLOT

nobase_include_HEADERS = Pulsar/PlotterFactory.h Pulsar/Graph.h \
	Pulsar/PlotAxis.h Pulsar/PlotAxisTI.h \
	Pulsar/PlotLabel.h Pulsar/PlotLabelTI.h \
	Pulsar/PlotFrame.h Pulsar/PlotFrameTI.h \
	Pulsar/ProfilePlotter.h Pulsar/ProfilePlotterTI.h \
	Pulsar/FluxPlotter.h Pulsar/FluxPlotterTI.h \
	Pulsar/SinglePlotter.h Pulsar/SinglePlotterTI.h \
	Pulsar/PosAngPlotter.h Pulsar/PosAngPlotterTI.h \
	Pulsar/PhaseVsPlotter.h Pulsar/PhaseVsPlotterTI.h \
	Pulsar/PhaseVsFrequency.h Pulsar/PhaseVsFrequencyTI.h \
	Pulsar/PhaseVsTime.h Pulsar/PhaseVsTimeTI.h

libPlotter_la_SOURCES = PlotterFactory.C \
	PlotAxis.C PlotAxisTI.C \
	PlotLabel.C PlotLabelTI.C \
	PlotFrame.C PlotFrameTI.C \
	ProfilePlotter.C ProfilePlotterTI.C \
	FluxPlotter.C FluxPlotterTI.C \
	SinglePlotter.C SinglePlotterTI.C \
	PosAngPlotter.C PosAngPlotterTI.C \
	PhaseVsPlotter.C PhaseVsPlotterTI.C \
	PhaseVsFrequency.C PhaseVsFrequencyTI.C \
	PhaseVsTime.C PhaseVsTimeTI.C

bin_PROGRAMS = psrplot

psrplot_SOURCES = psrplot.C

endif

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/utils/include -I$(top_builddir)/astro/include \
	-I$(top_srcdir)/psrchive -I$(top_srcdir)/psrchive/Extension \
	@PGPLOT_CFLAGS@ @SLA_CFLAGS@

LDADD = libPlotter.la $(top_builddir)/psrchive/libpsrchive.la	\
	$(top_builddir)/astro/libpsrastro.la			\
	$(top_builddir)/utils/libpsrutils.la @PGPLOT_LIBS@
