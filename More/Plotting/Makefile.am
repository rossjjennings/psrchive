
noinst_LTLIBRARIES = libPlot.la

##############################################################################
#
# Pretty much everything is PGPLOT-specific code
#
if HAVE_PGPLOT

nobase_include_HEADERS = Pulsar/Plot.h Pulsar/PlotFactory.h \
	Pulsar/PlotZoom.h Pulsar/PlotZoomTI.h \
	Pulsar/PlotAxis.h Pulsar/PlotAxisTI.h \
	Pulsar/PlotLabel.h Pulsar/PlotLabelTI.h \
	Pulsar/PlotFrame.h Pulsar/PlotFrameTI.h \
	Pulsar/PlotFrameSize.h Pulsar/PlotFrameSizeTI.h \
	Pulsar/MultiFrame.h Pulsar/MultiFrameTI.h \
	Pulsar/PhasePlot.h Pulsar/PhasePlotTI.h \
	Pulsar/FluxPlot.h Pulsar/FluxPlotTI.h \
	Pulsar/SinglePlot.h Pulsar/SinglePlotTI.h \
	Pulsar/StokesPlot.h Pulsar/StokesPlotTI.h \
	Pulsar/AnglePlot.h Pulsar/AnglePlotTI.h \
	Pulsar/PosAngPlot.h Pulsar/EllAngPlot.h \
	Pulsar/PhaseVsPlot.h Pulsar/PhaseVsPlotTI.h \
	Pulsar/PhaseVsFrequency.h Pulsar/PhaseVsFrequencyTI.h \
	Pulsar/PhaseVsTime.h Pulsar/PhaseVsTimeTI.h \
	Pulsar/MultiPhase.h \
	Pulsar/StokesCylindrical.h Pulsar/StokesCylindricalTI.h \
	Pulsar/StokesSpherical.h Pulsar/StokesSphericalTI.h

libPlot_la_SOURCES = PlotFactory.C \
	PlotZoom.C PlotZoomTI.C \
	PlotAxis.C PlotAxisTI.C \
	PlotLabel.C PlotLabelTI.C \
	PlotFrame.C PlotFrameTI.C \
	PlotFrameSize.C PlotFrameSizeTI.C \
	MultiFrame.C MultiFrameTI.C \
	PhasePlot.C PhasePlotTI.C \
	FluxPlot.C FluxPlotTI.C \
	SinglePlot.C SinglePlotTI.C \
	StokesPlot.C StokesPlotTI.C \
	AnglePlot.C AnglePlotTI.C \
	PosAngPlot.C EllAngPlot.C \
	PhaseVsPlot.C PhaseVsPlotTI.C \
	PhaseVsFrequency.C PhaseVsFrequencyTI.C \
	PhaseVsTime.C PhaseVsTimeTI.C \
	MultiPhase.C \
	StokesCylindrical.C StokesCylindricalTI.C \
	StokesSpherical.C StokesSphericalTI.C 

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

LDADD = libPlot.la $(top_builddir)/psrchive/libpsrchive.la	\
	$(top_builddir)/astro/libpsrastro.la			\
	$(top_builddir)/utils/libpsrutils.la @PGPLOT_LIBS@
