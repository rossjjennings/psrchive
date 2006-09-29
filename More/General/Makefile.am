
lib_LTLIBRARIES = libpsrchive.la

nobase_include_HEADERS = model_profile.h	\
	Pulsar/ArchiveExpert.h			\
	Pulsar/Algorithm.h			\
	Pulsar/Container.h			\
	Pulsar/AdaptiveSNR.h			\
	Pulsar/Archive.h			\
	Pulsar/ArchiveMatch.h			\
	Pulsar/ArchiveSort.h			\
	Pulsar/ArchiveProcessor.h		\
	Pulsar/PolnProfile.h			\
	Pulsar/BaselineEstimator.h		\
	Pulsar/DeltaPA.h			\
	Pulsar/DeltaRM.h			\
	Pulsar/ExponentialBaseline.h		\
	Pulsar/GaussianBaseline.h		\
	Pulsar/Profile.h			\
	Pulsar/BasicArchive.h			\
	Pulsar/BasicIntegration.h		\
	Pulsar/RFIMitigation.h			\
	Pulsar/ColdPlasma.h			\
	Pulsar/Dispersion.h			\
	Pulsar/DispersionDelay.h		\
	Pulsar/Fourier.h			\
	Pulsar/IntegrationManagerExpert.h	\
	Pulsar/IntegrationExpert.h		\
	Pulsar/Transformation.h			\
	Pulsar/Smooth.h				\
	Pulsar/ExampleArchive.h			\
	Pulsar/SmoothMean.h			\
	Pulsar/Polarization.h			\
	Pulsar/SmoothMedian.h			\
	Pulsar/FourierSNR.h			\
	Pulsar/StandardSNR.h			\
	Pulsar/OnPulseThreshold.h		\
	Pulsar/NoiseStatistics.h		\
	Pulsar/OnPulseEstimator.h		\
	Pulsar/PolnProfileStats.h		\
	Pulsar/InterpreterVariables.h		\
	Pulsar/SquareWave.h			\
	Pulsar/ScatteredPowerCorrection.h	\
	Pulsar/Integration.h			\
	Pulsar/Transposer.h			\
	Pulsar/IntegrationManager.h		\
	Pulsar/Utilities.h			\
	Pulsar/Interpreter.h			\
	Pulsar/psrchive.h			\
	Pulsar/IterativeBaseline.h		\
	Pulsar/PhaseWeight.h			\
	Pulsar/ArchiveTI.h			\
	Pulsar/PhaseWeightFunction.h		\
	Pulsar/ProfileWeightFunction.h		\
	Pulsar/Plotter.h			\
	Pulsar/IntegrationTI.h			\
	Pulsar/BaselineWindow.h			\
	Pulsar/Pulsar.h				\
	Pulsar/ReflectStokes.h			\
	Pulsar/ChannelZapMedian.h		\
	Pulsar/ChannelWeight.h			\
	Pulsar/ChannelZapModulation.h		\
	Pulsar/Config.h				\
	Pulsar/Profile_utils.h			\
	Pulsar/Archive_utils.h			\
	Pulsar/Weight.h				\
	Pulsar/IntegrationWeight.h		\
	Pulsar/DurationWeight.h			\
	Pulsar/SNRWeight.h

libpsrchive_la_SOURCES =			\
	AdaptiveSNR.C				\
	Agent.C					\
	Agent_static.C				\
	Archive_append.C			\
	Archive.C				\
	Archive_convert_state.C			\
	Archive_copy.C				\
	Archive_denoise_method.C		\
	Archive_fappend.C			\
	Archive_find.C				\
	Archive_get_PA.C			\
	Archive_init_Integration.C		\
	Archive_load.C				\
	Archive_match.C				\
	ArchiveMatch.C				\
	Archive_resize.C			\
	ArchiveSort.C				\
	Archive_split_method.C			\
	ArchiveTI.C				\
	Archive_toas.C				\
	Archive_transform.C			\
	Archive_unload.C			\
	Archive_update_model.C			\
	Archive_utils.C				\
	Archive_verbose.C			\
	BaselineWindow.C			\
	BasicArchive.C				\
	BasicIntegration.C			\
	ChannelZapMedian.C			\
	ChannelZapModulation.C			\
	choose.C				\
	DeltaPA.C				\
	DeltaRM.C				\
	Dispersion.C				\
	DispersionDelay.C			\
	DurationWeight.C			\
	ExampleArchive.C			\
	ExponentialBaseline.C			\
	Fourier.C				\
	FourierSNR.C				\
	GaussianBaseline.C			\
	getopt.h				\
	Integration.C				\
	Integration_cal_levels.C		\
	Integration_convert_state.C		\
	Integration_dedisperse.C		\
	Integration_fappend.C			\
	Integration_find.C			\
	Integration_get_PA.C			\
	Integration_get_Stokes.C		\
	Integration_invint.C			\
	IntegrationManager.C			\
	Integration_match.C			\
	Integration_new_PolnProfile.C		\
	Integration_remove_baseline.C		\
	Integration_resize.C			\
	IntegrationTI.C				\
	Integration_toas.C			\
	Integration_total.C			\
	Integration_transform.C			\
	Integration_weighted_frequency.C	\
	Interpreter.C				\
	InterpreterVariables.C			\
	IterativeBaseline.C			\
	LastSignificant.C			\
	model_profile.C				\
	mtm_analysis.C				\
	NoiseStatistics.C			\
	OnPulseThreshold.C			\
	pafit_get_PA.C				\
	PhaseGradShift.C			\
	PhaseWeight.C				\
	Physical.C				\
	Physical.h				\
	plugins.csh				\
	Polarization.C				\
	PolnProfile.C				\
	PolnProfileStats.C			\
	Profile_average.C			\
	Profile.C				\
	Profile_convolve.C			\
	Profile_denoise.C			\
	Profile_fftconv.C			\
	Profile_find_minmax_phase.C		\
	Profile_find_peak_edges.C		\
	Profile_find_transitions.C		\
	Profile_flux.C				\
	Profile_rotate.C			\
	Profile_shift.C				\
	Profile_snr.C				\
	Profile_snr_fortran.C			\
	Profile_stats.C				\
	Profile_toa.C				\
	Profile_utils.C				\
	ProfileWeightFunction.C			\
	ReflectStokes.C				\
	RFIMitigation.C				\
	ScatteredPowerCorrection.C		\
	SincInterpShift.C			\
	Smooth.C				\
	SmoothMean.C				\
	SmoothMedian.C				\
	SNRWeight.C				\
	SquareWave.C				\
	StandardSNR.C				\
	Transposer.C				\
	Utilities.C				\
	Weight.C				\
	ZeroPadShift.C

# ######################################################################
#
# programs
#
bin_PROGRAMS = psredit psrconv vip psr_template \
	fix_coords fix_fluxcal fix_hydra fix_3C353 \
	sesdiv parInfo default_loader archive_correct peb

psredit_SOURCES			= psredit.C
psrconv_SOURCES			= psrconv.C
vip_SOURCES			= vip.C
psr_template_SOURCES		= psr_template.C
fix_coords_SOURCES		= fix_coords.C
fix_fluxcal_SOURCES		= fix_fluxcal.C
fix_hydra_SOURCES		= fix_hydra.C
fix_3C353_SOURCES		= fix_3C353.C
sesdiv_SOURCES			= sesdiv.C
parInfo_SOURCES                 = parInfo.C
default_loader_SOURCES		= default_loader.C
archive_correct_SOURCES		= archive_correct.C
peb_SOURCES	         	= peb.C


# Config.C must be the last source code in the list
libpsrchive_la_SOURCES += Config.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/third/include				  \
	-I$(top_builddir)/utils/include					  \
	-I$(top_builddir)/tempo/include					  \
        -I$(top_srcdir)/psrchive/File
	-I$(top_srcdir)/psrchive/File/Extension \
	@PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @MPI_CFLAGS@

LDADD = libpsrchive.la $(top_builddir)/astro/libpsrtempo.la \
        $(top_builddir)/utils/libpsrutils.la \
	$(top_builddir)/third/libpsrthird.la

include $(top_srcdir)/config/Makefile.local

