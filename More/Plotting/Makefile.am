
noinst_LTLIBRARIES = libPlotting.la

##############################################################################
#
# Pretty much everything is PGPLOT-specific code
#
if HAVE_PGPLOT

nobase_include_HEADERS = iopair.h					\
	Pulsar/Plot.h Pulsar/PlotFactory.h Pulsar/PlotIndex.h		\
	Pulsar/PlotScale.h Pulsar/PhaseScale.h Pulsar/FrequencyScale.h	\
	Pulsar/PlotAxis.h Pulsar/PlotLabel.h Pulsar/PlotAttributes.h	\
	Pulsar/PlotFrame.h Pulsar/PlotFrameSize.h Pulsar/MultiFrame.h	\
	Pulsar/SimplePlot.h Pulsar/FrequencyPlot.h Pulsar/PhasePlot.h	\
	Pulsar/PowerSpectra.h Pulsar/SpectrumPlot.h			\
	Pulsar/FluxPlot.h Pulsar/ProfilePlot.h Pulsar/StokesPlot.h	\
	Pulsar/AnglePlot.h Pulsar/PosAngPlot.h Pulsar/EllAngPlot.h	\
	Pulsar/PhaseVsPlot.h Pulsar/PhaseVsTime.h			\
	Pulsar/PhaseVsFrequency.h Pulsar/PhaseVsFrequencyPlus.h		\
	Pulsar/MultiPlot.h Pulsar/MultiPhase.h				\
	Pulsar/StokesCylindrical.h Pulsar/StokesSpherical.h		\
	Pulsar/FluctPlot.h Pulsar/StokesFluctPlot.h			\
	Pulsar/Poincare.h Pulsar/ProfileVectorPlotter.h			\
	Pulsar/TwoBitPlot.h Pulsar/CalibratorSpectrum.h

libPlotting_la_SOURCES = Plot.C PlotFactory.C \
	PlotIndex.C \
	PlotScale.C PlotScaleTI.C \
	PhaseScale.C PhaseScaleTI.C \
	FrequencyScale.C \
	PlotAxis.C PlotAxisTI.C \
	PlotLabel.C PlotLabelTI.C \
	PlotAttributes.C PlotAttributesTI.C \
	PlotFrame.C PlotFrameTI.C \
	PlotFrameSize.C PlotFrameSizeTI.C \
	MultiFrame.C MultiFrameTI.C \
	SimplePlot.C FrequencyPlot.C \
	PowerSpectra.C PowerSpectraTI.C \
	SpectrumPlot.C SpectrumPlotTI.C \
	PhasePlot.C PhasePlotTI.C \
	FluxPlot.C FluxPlotTI.C \
	ProfilePlot.C ProfilePlotTI.C \
	StokesPlot.C StokesPlotTI.C \
	AnglePlot.C AnglePlotTI.C \
	PosAngPlot.C EllAngPlot.C \
	PhaseVsPlot.C PhaseVsPlotTI.C \
	PhaseVsFrequency.C PhaseVsFrequencyTI.C \
	PhaseVsTime.C PhaseVsTimeTI.C \
	MultiPlot.C \
	MultiPhase.C MultiPhaseTI.C\
	StokesCylindrical.C StokesCylindricalTI.C \
	StokesSpherical.C StokesSphericalTI.C \
	PhaseVsFrequencyPlus.C PhaseVsFrequencyPlusTI.C \
	FluctPlot.C FluctPlotTI.C \
	StokesFluctPlot.C StokesFluctPlotTI.C \
	TwoBitPlot.C TwoBitPlotTI.C \
	Poincare.C PoincareTI.C \
	ProfileVectorPlotter.C \
	CalibratorSpectrum.C CalibratorSpectrumTI.C

endif

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/utils/include -I$(top_builddir)/Core/include \
	-I$(top_srcdir)/More/General -I$(top_srcdir)/More/Polarimetry \
	@PGPLOT_CFLAGS@ @SLA_CFLAGS@

