
noinst_LTLIBRARIES = libGeneral.la

dist_data_DATA = fluxcal.cfg

nobase_include_HEADERS = \
	find_spike_edges.h \
        model_profile.h \
        morphological_difference.h \
	Pulsar/Append.h \
	Pulsar/counter_drift.h \
	Pulsar/denoise.h \
	Pulsar/shift_methods.h \
        Pulsar/AdaptiveSNR.h \
        Pulsar/Algorithm.h \
        Pulsar/ArchiveMatch.h \
        Pulsar/ArchiveSort.h \
        Pulsar/BaselineEstimator.h \
        Pulsar/BaselineWindow.h \
        Pulsar/ChannelWeight.h \
        Pulsar/ChannelZapMedian.h \
        Pulsar/ChannelZapModulation.h \
        Pulsar/ColdPlasma.h \
        Pulsar/DispersionDelay.h \
        Pulsar/Dispersion.h \
	Pulsar/Divided.h \
        Pulsar/DurationWeight.h \
	Pulsar/EvenlySpaced.h \
	Pulsar/EvenlyWeighted.h \
        Pulsar/ExponentialBaseline.h \
	Pulsar/FixFluxCal.h \
        Pulsar/Fourier.h \
        Pulsar/FourierSNR.h \
	Pulsar/FrequencyAppend.h \
        Pulsar/FrequencyIntegrate.h \
        Pulsar/GaussianBaseline.h \
	Pulsar/InstallInterpreter.h \
        Pulsar/InstrumentInfo.h \
	Pulsar/Integrate.h \
        Pulsar/IntegrationWeight.h \
        Pulsar/Interpreter.h \
	Pulsar/InterpreterExtension.h \
        Pulsar/InterpreterVariables.h \
        Pulsar/IterativeBaseline.h \
	Pulsar/MaskSmooth.h \
        Pulsar/NoiseStatistics.h \
        Pulsar/OnPulseEstimator.h \
        Pulsar/OnPulseThreshold.h \
        Pulsar/PhaseWeightFunction.h \
	Pulsar/PhaseWeightModifier.h \
	Pulsar/PhaseWeightSmooth.h \
        Pulsar/PhaseWeight.h \
        Pulsar/Polarization.h \
        Pulsar/PolnProfileFit.h \
        Pulsar/PolnProfile.h \
        Pulsar/PolnProfileStats.h \
        Pulsar/ProfileWeightFunction.h \
        Pulsar/RFIMitigation.h \
        Pulsar/ScatteredPowerCorrection.h \
        Pulsar/SimPolnTiming.h \
        Pulsar/Smooth.h \
        Pulsar/SmoothMean.h \
        Pulsar/SmoothMedian.h \
        Pulsar/SNRWeight.h \
        Pulsar/SourceInfo.h \
        Pulsar/SquareWave.h \
	Pulsar/StandardCandles.h \
        Pulsar/StandardSNR.h \
        Pulsar/SyntheticPolnProfile.h \
	Pulsar/TimeAppend.h \
	Pulsar/TimeIntegrate.h \
        Pulsar/Transformation.h \
        Pulsar/Transposer.h \
        Pulsar/Weight.h \
        Pulsar/ZapInterpreter.h

libGeneral_la_SOURCES = \
	counter_drift.C \
	denoise.C \
	find_spike_edges.C \
        model_profile.C \
	morphological_difference.C \
        AdaptiveSNR.C \
	Append.C \
        Archive_append.C \
	Archive_apply_model.C \
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
        Archive_match.C \
        ArchiveMatch.C \
        Archive_pscrunch.C \
        Archive_remove_baseline.C \
        Archive_rms_baseline.C \
        Archive_rotate.C \
        Archive_set_ephemeris.C \
        Archive_set_model.C \
        ArchiveSort.C \
        Archive_toas.C \
        Archive_total.C \
        Archive_transform.C \
        Archive_tscrunch.C \
        Archive_update_model.C \
        BaselineWindow.C \
        ChannelZapMedian.C \
        ChannelZapModulation.C \
        Dispersion.C \
        DispersionDelay.C \
        DurationWeight.C \
        ExponentialBaseline.C \
	FixFluxCal.C \
        Fourier.C \
        FourierSNR.C \
	FrequencyAppend.C \
        FrequencyIntegrate.C \
        GaussianBaseline.C \
	InstallInterpreter.C \
        Integration_cal_levels.C \
        Integration_convert_state.C \
        Integration_dedisperse.C \
        Integration_find.C \
        Integration_fscrunch.C \
        Integration_get_Stokes.C \
        Integration_invint.C \
        Integration_new_PolnProfile.C \
        Integration_remove_baseline.C \
        Integration_rotate.C \
        Integration_toas.C \
        Integration_total.C \
        Integration_transform.C \
        Integration_weighted_frequency.C \
	Interpreter.C \
	InterpreterVariables.C \
        IterativeBaseline.C \
        LastSignificant.C \
	MaskSmooth.C \
        NoiseStatistics.C \
	OnPulseThreshold.C \
	PhaseWeightModifier.C \
        PhaseWeight.C \
        Profile_convolve.C \
        Profile_dedisperse.C \
        Profile_fftconv.C \
        Profile_find_minmax_phase.C \
        Profile_find_peak_edges.C \
        Profile_find_transitions.C \
        Profile_rotate.C \
        Profile_shift.C \
        Profile_snr.C \
        Profile_stats.C \
        Profile_toa.C \
        ProfileWeightFunction.C \
        RFIMitigation.C \
        ScatteredPowerCorrection.C \
        Smooth.C \
        SmoothMean.C \
        SmoothMedian.C \
        SNRWeight.C \
        SquareWave.C \
        StandardCandles.C \
        StandardSNR.C \
	TimeAppend.C \
	TimeIntegrate.C \
        Transposer.C \
        Weight.C \
	ZapInterpreter.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include @TEMPO2_CFLAGS@

include $(top_srcdir)/config/Makefile.include

