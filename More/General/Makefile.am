
noinst_LTLIBRARIES = libGeneral.la

dist_data_DATA = fluxcal.cfg

nobase_include_HEADERS = \
	find_spike_edges.h \
        model_profile.h \
        morphological_difference.h \
	Pulsar/Append.h \
	Pulsar/counter_drift.h \
	Pulsar/Accumulate.h \
        Pulsar/AdaptiveSNR.h \
        Pulsar/AdaptiveSmooth.h \
        Pulsar/Algorithm.h \
	Pulsar/ArchiveComparisons.h \
        Pulsar/ArchiveSort.h \
	Pulsar/ArchiveStatistic.h \
	Pulsar/ArchiveTemplates.h \
        Pulsar/BaselineEstimator.h \
        Pulsar/BaselineWindow.h \
        Pulsar/ChannelWeight.h \
        Pulsar/ColdPlasma.h \
	Pulsar/Combination.h \
	Pulsar/CompareWith.h \
	Pulsar/CompareWithEachOther.h \
	Pulsar/CompareWithSum.h \
	Pulsar/Contemporaneity.h \
	Pulsar/ConvertIsolated.h \
	Pulsar/Convolve.h \
	Pulsar/Correlate.h \
	Pulsar/DeleteInterpreter.h \
	Pulsar/Detrend.h \
	Pulsar/Differentiate.h \
	Pulsar/DisperseWeight.h \
        Pulsar/DispersionDelay.h \
        Pulsar/Dispersion.h \
	Pulsar/Divided.h \
        Pulsar/DurationWeight.h \
        Pulsar/DynamicSpectrum.h \
	Pulsar/EvenlySpaced.h \
	Pulsar/EvenlyWeighted.h \
        Pulsar/ExponentialBaseline.h \
	Pulsar/ExtensionInterpreter.h \
	Pulsar/Extract.h \
	Pulsar/FixFluxCal.h \
	Pulsar/FixInterpreter.h \
	Pulsar/Flux.h \
	Pulsar/ForEachProfile.h \
	Pulsar/FortranSNR.h \
        Pulsar/Fourier.h \
        Pulsar/FourierSNR.h \
	Pulsar/FrequencyAppend.h \
        Pulsar/FrequencyIntegrate.h \
	Pulsar/FscrunchInterpreter.h \
        Pulsar/GaussianBaseline.h \
	Pulsar/HasArchive.h \
	Pulsar/HasBaselineEstimator.h \
	Pulsar/ImageCorrection.h \
	Pulsar/Index.h \
	Pulsar/Integrate.h \
	Pulsar/IntegrationBarycentre.h \
	Pulsar/InstallInterpreter.h \
        Pulsar/IntegrationWeight.h \
        Pulsar/Interpreter.h \
	Pulsar/InterpreterExtension.h \
	Pulsar/IQRBaseline.h \
        Pulsar/IterativeBaseline.h \
	Pulsar/LastHarmonic.h \
	Pulsar/LastSignificant.h \
	Pulsar/MaskSmooth.h \
	Pulsar/ModularSNR.h \
        Pulsar/NoiseStatistics.h \
	Pulsar/NonlinearlySpaced.h \
        Pulsar/OnPulseEstimator.h \
        Pulsar/OnPulseThreshold.h \
	Pulsar/PatchFrequency.h \
	Pulsar/PatchTime.h \
	Pulsar/PeakEdgesInterpreter.h \
	Pulsar/PeakConsecutive.h \
	Pulsar/PeakCumulative.h \
	Pulsar/PhaseSNR.h \
        Pulsar/PhaseWeightFunction.h \
	Pulsar/PhaseWeightInterface.h \
	Pulsar/PhaseWeightModifier.h \
	Pulsar/PhaseWeightShift.h \
	Pulsar/PhaseWeightSmooth.h \
	Pulsar/PhaseWeightStatistic.h \
        Pulsar/PhaseWeight.h \
	Pulsar/PhaseWidth.h \
	Pulsar/ProfileCovariance.h \
	Pulsar/ProfileInterpreter.h \
	Pulsar/ProfilePCA.h \
	Pulsar/ProfileShiftFit.h \
	Pulsar/ProfileStatistic.h \
	Pulsar/ProfileStats.h \
	Pulsar/ProfileStatsInterface.h \
        Pulsar/ProfileWeightFunction.h \
	Pulsar/ProfileWeightStatic.h \
	Pulsar/RadiometerWeight.h \
	Pulsar/RemoveBaseline.h \
	Pulsar/RemoveVariableBaseline.h \
	Pulsar/RiseFall.h \
        Pulsar/RFIMitigation.h \
        Pulsar/ScatteredPowerCorrection.h \
	Pulsar/ScrunchFactor.h \
        Pulsar/SimPolnTiming.h \
        Pulsar/Smooth.h \
        Pulsar/SmoothMean.h \
        Pulsar/SmoothMedian.h \
	Pulsar/SmoothSinc.h \
	Pulsar/SNRatioEstimator.h \
        Pulsar/SNRWeight.h \
        Pulsar/SquareWave.h \
	Pulsar/StandardCandles.h \
        Pulsar/StandardFlux.h \
        Pulsar/StandardSNR.h \
        Pulsar/StandardSNRWeight.h \
	Pulsar/Statistics.h \
	Pulsar/StatisticsInterface.h \
	Pulsar/Subtract.h \
        Pulsar/SyntheticPolnProfile.h \
	Pulsar/TargetDuration.h \
	Pulsar/TimeAppend.h \
	Pulsar/TimeDomainCovariance.h \
	Pulsar/TimeIntegrate.h \
        Pulsar/Transformation.h \
        Pulsar/Transposer.h \
        Pulsar/WaveletSmooth.h \
        Pulsar/WaveletTransform.h \
        Pulsar/Weight.h \
	Pulsar/WeightedFrequency.h \
	Pulsar/WeightInterpreter.h \
	Pulsar/WidthEstimator.h

libGeneral_la_SOURCES = \
	counter_drift.C \
	find_spike_edges.C \
        model_profile.C \
	morphological_difference.C \
	Accumulate.C \
        AdaptiveSNR.C \
	Append.C \
	ArchiveComparisons.C \
	ArchiveStatistic.C \
	ArchiveTemplates.C \
        Archive_append.C \
	Archive_apply_model.C \
	Archive_baseline.C \
        Archive_bscrunch.C \
        Archive_centre.C \
        Archive_centre_max_bin.C \
        Archive_convert_state.C \
        Archive_dedisperse.C \
        Archive_defaraday.C \
        Archive_find.C \
        Archive_fold.C \
        Archive_fscrunch.C \
	Archive_good_model.C \
        Archive_invint.C \
        Archive_pscrunch.C \
        Archive_remove_baseline.C \
        Archive_rms_baseline.C \
        Archive_rotate.C \
        Archive_set_ephemeris.C \
        Archive_set_model.C \
        ArchiveSort.C \
	Archive_total.C \
        Archive_transform.C \
        Archive_tscrunch.C \
	Archive_tscrunched.C \
        Archive_update_model.C \
	Archive_weighted_frequency.C \
	BaselineEstimator.C \
        BaselineWindow.C \
	CompareWith.C \
	CompareWithEachOther.C \
	CompareWithSum.C \
	ConvertIsolated.C \
	Contemporaneity.C \
	Convolve.C \
	Correlate.C \
	StrategySet.C \
	StrategySet_defaults.C \
	DeleteInterpreter.C \
	Detrend.C \
	Differentiate.C \
	DisperseWeight.C \
        Dispersion.C \
        DispersionDelay.C \
        DurationWeight.C \
        DynamicSpectrum.C \
	ExtensionInterpreter.C \
        ExponentialBaseline.C \
	Extract.C \
	FixFluxCal.C \
	FixInterpreter.C \
	ForEachProfile.C \
	FortranSNR.C \
        Fourier.C \
        FourierSNR.C \
	FrequencyAppend.C \
        FrequencyIntegrate.C \
        GaussianBaseline.C \
	HasArchive.C \
	HasBaselineEstimator.C \
	ImageCorrection.C \
	Index.C \
	InstallInterpreter.C \
        Integration_cal_levels.C \
        Integration_convert_state.C \
        Integration_dedisperse.C \
        Integration_find.C \
        Integration_fscrunch.C \
        Integration_get_Stokes.C \
        Integration_invint.C \
        Integration_new_PolnProfile.C \
	Integration_orphan.C \
        Integration_remove_baseline.C \
        Integration_rotate.C \
        Integration_total.C \
        Integration_transform.C \
        Integration_weighted_frequency.C \
	IntegrationBarycentre.C \
	IntegrationWeight.C \
	Interpreter.C \
	IQRBaseline.C \
        IterativeBaseline.C \
	LastHarmonic.C \
        LastSignificant.C \
	MaskSmooth.C \
	ModularSNR.C \
        NoiseStatistics.C \
	OnPulseThreshold.C \
	PatchFrequency.C \
	PatchTime.C \
	PeakEdgesInterpreter.C \
	PeakConsecutive.C \
	PeakCumulative.C \
	PhaseSNR.C \
	PhaseWeightInterface.C \
	PhaseWeightModifier.C \
	PhaseWeightShift.C \
	PhaseWeightStatistic.C \
        PhaseWeight.C \
	PhaseWidth.C \
	ProfileCovariance.C \
	ProfileInterpreter.C \
	ProfileShiftFit.C \
	ProfileStatistic.C \
	ProfileStats.C \
	ProfileStatsInterface.C \
	Profile_baseline.C \
        Profile_convolve.C \
        Profile_dedisperse.C \
        Profile_fftconv.C \
        Profile_find_minmax_phase.C \
        Profile_find_peak_edges.C \
        Profile_find_transitions.C \
        Profile_shift.C \
        Profile_snr.C \
        Profile_stats.C \
        ProfileWeightFunction.C \
	ProfileWeightStatic.C \
	RadiometerWeight.C \
	RemoveBaseline.C \
	RemoveVariableBaseline.C \
	RiseFall.C \
        RFIMitigation.C \
        ScatteredPowerCorrection.C \
	ScrunchFactor.C \
        Smooth.C \
        SmoothMean.C \
        SmoothMedian.C \
	SmoothSinc.C \
	SNRatioEstimator.C \
        SNRWeight.C \
        SquareWave.C \
	standard_interface.C \
        StandardCandles.C \
        StandardFlux.C \
        StandardSNR.C \
        StandardSNRWeight.C \
	Statistics.C \
	StatisticsInterface.C \
	Subtract.C \
	TargetDuration.C \
	TimeAppend.C \
	TimeDomainCovariance.C \
	TimeIntegrate.C \
        Transposer.C \
        Weight.C \
	WeightedFrequency.C \
	WeightInterpreter.C \
	width.C \
	WidthEstimator.C

##############################################################################
#
# GSL-dependent code
#
if HAVE_GSL

libGeneral_la_SOURCES += WaveletSmooth.C WaveletTransform.C AdaptiveSmooth.C 

if HAVE_CFITSIO

libGeneral_la_SOURCES += ProfilePCA.C

endif
endif

##############################################################################
#
# SPLINTER-dependent code
#
if HAVE_SPLINTER

libGeneral_la_SOURCES += SplineSmooth.C \
	BootstrapUncertainty2D.C \
	CrossValidatedSmooth2D.C

nobase_include_HEADERS += Pulsar/SplineSmooth.h

endif

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += \
        -I$(top_srcdir)/More/Timing \
        -I$(top_srcdir)/More/Polarimetry \
        -I$(top_srcdir)/More/MEAL \
        -I$(top_srcdir)/More/RFIMitigation \
	@PGPLOT_CFLAGS@ @TEMPO2_CFLAGS@ @CFITSIO_CFLAGS@ @GSL_CFLAGS@ \
	@SPLINTER_CFLAGS@ @ARMADILLO_CFLAGS@

libGeneral_la_LIBADD = @SPLINTER_LIBS@ @ARMADILLO_LIBS@

