
noinst_LTLIBRARIES = libPlotting.la

##############################################################################
#
# Pretty much everything is PGPLOT-specific code
#
if HAVE_PGPLOT

nobase_include_HEADERS =					       \
	Pulsar/Plot.h Pulsar/PlotFactory.h			       \
	Pulsar/PlotLoop.h			       \
	Pulsar/PlotEdge.h					       \
	Pulsar/PlotScale.h Pulsar/PhaseScale.h Pulsar/FrequencyScale.h \
	Pulsar/PlotAxis.h Pulsar/PlotLabel.h Pulsar/PlotAttributes.h   \
	Pulsar/PlotFrame.h Pulsar/MultiFrame.h Pulsar/FramedPlot.h     \
	Pulsar/SimplePlot.h Pulsar/FrequencyPlot.h Pulsar/PhasePlot.h  \
	Pulsar/PowerSpectra.h Pulsar/SpectrumPlot.h		       \
	Pulsar/FluxPlot.h Pulsar/ProfilePlot.h Pulsar/StokesPlot.h     \
	Pulsar/AnglePlot.h Pulsar/PosAngPlot.h Pulsar/EllAngPlot.h     \
	Pulsar/PhaseVsPlot.h Pulsar/PhaseVsTime.h		       \
	Pulsar/PhaseVsFrequency.h Pulsar/PhaseVsFrequencyPlus.h	       \
	Pulsar/MultiPlot.h Pulsar/MultiPhase.h			       \
	Pulsar/StokesCylindrical.h Pulsar/StokesSpherical.h	       \
	Pulsar/FluctPlot.h Pulsar/StokesFluctPlot.h		       \
	Pulsar/Poincare.h Pulsar/ProfileVectorPlotter.h		       \
	Pulsar/TwoBitPlot.h Pulsar/CalibratorSpectrum.h		       \
	Pulsar/LinePhasePlot.h					       \
	Pulsar/DynamicSpectrum.h				       \
	Pulsar/DynamicSNSpectrum.h				       \
	Pulsar/DynamicBaselineSpectrum.h			       \
	Pulsar/DynamicCalSpectrum.h				       \
	Pulsar/FreqVsSNRPlot.h					       \
	Pulsar/CalPhVFreqPlot.h					       \
	Pulsar/BandpassPlot.h					       \
	Pulsar/ChannelWeightsPlot.h				       \
	Pulsar/BandpassChannelWeightPlot.h			       \
	Pulsar/DigitiserStatsPlot.h				       \
	Pulsar/DigitiserCountsPlot.h				       \
	Pulsar/InfoLabel.h

libPlotting_la_SOURCES = Plot.C PlotFactory.C FramedPlot.C \
	PlotLoop.C \
	PlotEdgeTI.C \
	PlotScale.C PlotScaleTI.C \
	PhaseScale.C PhaseScaleTI.C \
	FrequencyScale.C \
	PlotAxis.C PlotAxisTI.C \
	PlotLabel.C PlotLabelTI.C InfoLabel.C \
	PlotAttributes.C PlotAttributesTI.C \
	PlotFrame.C PlotFrameTI.C \
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
	LinePhasePlot.C LinePhasePlotTI.C \
	DynamicSpectrum.C DynamicSpectrumTI.C \
	DynamicSNSpectrum.C DynamicSNSpectrumTI.C \
	DynamicBaselineSpectrum.C DynamicBaselineSpectrumTI.C \
	DynamicCalSpectrum.C DynamicCalSpectrumTI.C \
	CalibratorSpectrum.C CalibratorSpectrumTI.C \
	FreqVsSNRPlot.C FreqVsSNRPlotTI.C \
	BandpassPlot.C BandpassPlotTI.C \
	ChannelWeightsPlot.C ChannelWeightsPlotTI.C \
	BandpassChannelWeightPlot.C BandpassChannelWeightPlotTI.C \
	DigitiserStatsPlot.C DigitiserStatsPlotTI.C \
	DigitiserCountsPlot.C DigitiserCountsPlotTI.C \
	CalPhVFreqPlot.C CalPhVFreqPlotTI.C

endif

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@

