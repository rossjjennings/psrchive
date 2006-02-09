
lib_LTLIBRARIES = libpsrchive.la

nobase_include_HEADERS = model_profile.h \
	Pulsar/AdaptiveSNR.h             Pulsar/PolarCalibrator.h \
	Pulsar/Archive.h                 Pulsar/PolnCalibrator.h \
	Pulsar/ArchiveMatch.h            Pulsar/ArchiveSort.h \
	Pulsar/ArchiveProcessor.h        Pulsar/PolnProfile.h \
	Pulsar/BaselineEstimator.h       Pulsar/PolnProfileFit.h \
	Pulsar/DeltaRM.h                 Pulsar/DoPCalibrator.h \
	Pulsar/ExponentialBaseline.h     Pulsar/PolnProfileFitAnalysis.h \
	Pulsar/GaussianBaseline.h        Pulsar/Profile.h \
	Pulsar/BasicArchive.h            Pulsar/PulsarCalibrator.h \
	Pulsar/BasicIntegration.h        Pulsar/RFIMitigation.h \
	Pulsar/Calibrator.h              Pulsar/ReceptionCalibrator.h \
	Pulsar/CalibratorPlotter.h       Pulsar/ReceptionCalibratorPlotter.h \
	Pulsar/CalibratorStokesInfo.h    Pulsar/ReferenceCalibrator.h \
	Pulsar/CorrectionsCalibrator.h   Pulsar/SingleAxisCalibrator.h \
	Pulsar/IonosphereCalibrator.h \
	Pulsar/Database.h                Pulsar/Smooth.h \
	Pulsar/ExampleArchive.h          Pulsar/SmoothMean.h \
	Pulsar/FluxCalibrator.h          Pulsar/SmoothMedian.h \
	Pulsar/FluxCalibratorDatabase.h  Pulsar/SourceInfo.h \
	Pulsar/FourierSNR.h              Pulsar/StandardSNR.h \
	Pulsar/FrequencyIntegrate.h      Pulsar/OnPulseThreshold.h \
	Pulsar/OnPulseEstimator.h        Pulsar/PolnProfileStats.h \
	Pulsar/HybridCalibrator.h        Pulsar/SyntheticProfile.h \
	Pulsar/SyntheticPolnProfile.h    Pulsar/SimPolnTiming.h \
	Pulsar/InstrumentInfo.h          Pulsar/SystemCalibrator.h \
	Pulsar/Integration.h             Pulsar/Transposer.h \
	Pulsar/IntegrationManager.h      Pulsar/Utilities.h \
	Pulsar/IterativeBaseline.h       Pulsar/OffPulseCalibrator.h \
	Pulsar/PhaseWeight.h             Pulsar/ArchiveTI.h \
	Pulsar/PhaseWeightFunction.h     Pulsar/ProfileWeightFunction.h \
	Pulsar/Plotter.h                 Pulsar/IntegrationTI.h \
	Pulsar/ExtensionTI.h             Pulsar/BaselineWindow.h \
	Pulsar/ReflectStokes.h           Pulsar/ChannelZapMedian.h \
	Pulsar/ChannelWeight.h           Pulsar/ChannelZapModulation.h \
        Pulsar/Container.h               Pulsar/Pulsar.h

libpsrchive_la_SOURCES = plugins.csh getopt.h Physical.h \
	AdaptiveSNR.C                 Integration_new_PolnProfile.C  \
	Agent.C                       Integration_remove_baseline.C  \
	Agent_static.C                Integration_resize.C  \
	Archive.C                     Integration_toas.C  \
	Archive_append.C              Integration_total.C  \
	Archive_convert_state.C       Integration_transform.C  \
	Archive_copy.C                Integration_weighted_frequency.C  \
	Archive_correct_instrument.C  PhaseWeight.C  \
	Archive_denoise_method.C      Physical.C  \
	Archive_fappend.C             PolarCalibrator.C  \
	Archive_find.C                PolnCalibrator.C  \
	Archive_get_PA.C              PolnCalibratorInfo.C  \
	Archive_load.C                PolnProfile.C  \
	Archive_match.C               PolnProfileFit.C  \
	ArchiveMatch.C                ArchiveSort.C \
        PolnProfileFitAnalysis.C      OffPulseCalibrator.C \
	OnPulseThreshold.C            PolnProfileStats.C \
	Archive_resize.C              Profile.C  \
	Archive_split_method.C        Profile_average.C  \
	Archive_toas.C                Profile_basic_shift.C  \
	Archive_total.C               Profile_convolve.C  \
	Archive_transform.C           Profile_denoise.C  \
	Profile_find_minmax_phase.C   ReflectStokes.C \
	Archive_tscrunch.C            Profile_find_peak_edges.C  \
	Archive_unload.C              Profile_find_transitions.C  \
	Archive_update_model.C        Profile_flux.C  \
	Archive_verbose.C             Profile_rotate.C  \
	GaussianBaseline.C            Profile_shift.C  \
	BasicArchive.C                Profile_snr.C  \
	BasicIntegration.C            Profile_snr_fortran.C  \
	Calibrator.C                  Profile_stats.C  \
	CalibratorStokesInfo.C        Profile_toa.C  \
	CorrectionsCalibrator.C       PulsarCalibrator.C  \
	Database.C                    RFIMitigation.C  \
	DeltaRM.C                     DoPCalibrator.C \
	ExampleArchive.C              ReceptionCalibrator.C  \
	ExponentialBaseline.C         FrequencyIntegrate.C \
	FluxCalibrator.C              ReceptionCalibrator_get_Info.C  \
	FluxCalibratorDatabase.C      ReceptionCalibrator_get_solution.C  \
	FourierSNR.C                  ReferenceCalibrator.C  \
	HybridCalibrator.C            SingleAxisCalibrator.C  \
	InstrumentInfo.C              Smooth.C  \
	Integration.C                 SmoothMean.C  \
	IntegrationManager.C          SmoothMedian.C  \
	Integration_cal_levels.C      SourceInfo.C  \
	Integration_convert_state.C   StandardSNR.C  \
	Integration_dedisperse.C      SyntheticProfile.C  \
	SyntheticPolnProfile.C        SimPolnTiming.C \
	Integration_defaraday.C       SystemCalibrator.C  \
	Integration_fappend.C         Transposer.C  \
	Integration_find.C            Utilities.C  \
	Integration_flux.C            choose.C  \
	Integration_fscrunch.C        GaussianShift.C \
	Integration_get_PA.C          ZeroPadShift.C \
	Integration_get_Stokes.C      model_profile.C  \
	Integration_invint.C          pafit_get_PA.C  \
	Integration_match.C           ArchiveTI.C \
	IntegrationTI.C               ArchiveTI_import.C \
	IterativeBaseline.C           ProfileWeightFunction.C \
	IonosphereCalibrator.C \
	PhaseGradShift.C              Profile_fftconv.C \
	BaselineWindow.C              ChannelZapMedian.C \
	ChannelWeight.C               ChannelZapModulation.C \
        SincInterpShift.C

if !HAVE_GETOPT_LONG
libpsrchive_la_SOURCES += getopt.c getopt1.c
endif

SUBDIRS                 = Calibration Extension timer epn ascii

libpsrchive_la_LIBADD   = Calibration/libCalibration.la \
			Extension/libExtension.la \
			timer/libtimer.la epn/libepn.la ascii/libascii.la

if HAVE_CFITSIO
SUBDIRS += psrfits
libpsrchive_la_LIBADD += psrfits/libpsrfits.la
endif

SUBDIRS += .

if HAVE_QT
if HAVE_PGPLOT
SUBDIRS += GUI
endif
endif

if HAVE_QTDRIV
SUBDIRS += rhythm
endif

# ######################################################################
#
# programs
#
bin_PROGRAMS = psr_template pac paz vap pcq vip psredit psradd fadd pat pam \
        psrdiff fluxcal fix_coords test_Difference fix_hydra sesdiv \
        fix_3C353 archive_correct archive_denoise \
        fix_fluxcal archive_split psrconv pascii \
        default_loader parInfo 


psr_template_SOURCES		= psr_template.C
archive_correct_SOURCES		= archive_correct.C
archive_denoise_SOURCES		= archive_denoise.C
archive_split_SOURCES		= archive_split.C
default_loader_SOURCES		= default_loader.C
fadd_SOURCES			= fadd.C
fix_3C353_SOURCES		= fix_3C353.C
fix_coords_SOURCES		= fix_coords.C
fix_fluxcal_SOURCES		= fix_fluxcal.C
fix_hydra_SOURCES		= fix_hydra.C
fluxcal_SOURCES			= fluxcal.C
paz_SOURCES			= paz.C
pac_SOURCES			= pac.C
pam_SOURCES			= pam.C
pascii_SOURCES			= pascii.C
pat_SOURCES			= pat.C
psradd_SOURCES			= psradd.C
psrconv_SOURCES			= psrconv.C
sesdiv_SOURCES			= sesdiv.C
vap_SOURCES			= vap.C
vip_SOURCES			= vip.C
pcq_SOURCES			= pcq.C
psredit_SOURCES			= psredit.C
parInfo_SOURCES                 = parInfo.C
psrdiff_SOURCES			= psrdiff.C

TESTS = test_Receiver
check_PROGRAMS = test_Archive test_denoise test_Difference $(TESTS)

test_Receiver_SOURCES		= test_Receiver.C
test_Archive_SOURCES		= test_Archive.C
test_denoise_SOURCES		= test_denoise.C
test_Difference_SOURCES		= test_Difference.C

# ######################################################################
#
# PGPLOT specific code
#
if HAVE_PGPLOT

libpsrchive_la_SOURCES += ArchiveProcessor.C Plotter.C CalibratorPlotter.C  \
	ReceptionCalibratorPlotter.C   plotJones.C\
	Plotter_Mercator.C             Plotter_phase_subints.C  \
	Plotter_baseline_spectrum.C    Plotter_plot.C  \
	Plotter_calibrator_spectrum.C  Plotter_set_colour_map.C  \
	Plotter_draw_transitions.C     Plotter_simple_ds.C  \
	Plotter_phase_fourier.C        Plotter_spherical.C  \
	Plotter_phase_frequency.C      Plotter_phase_time.C \
	Plotter_draw_colour_map.C

# Plotter_pseudo_vector_correlation.C

bin_PROGRAMS += pacv pcvar pav pas psrwt psrsh pcm pvt paf psrqft \
	binphs_test pcm_chooser rmfit psrband SimTiming bin_clean \
	plot_baseline pdmp psrfi tsys spa psrchive_template aas pfixbaseline

SimTiming_SOURCES		= SimTiming.C
binphs_test_SOURCES		= binphs_test.C
pacv_SOURCES			= pacv.C
pas_SOURCES			= pas.C
pav_SOURCES			= pav.C
pcm_SOURCES			= pcm.C
pcm_chooser_SOURCES		= pcm_chooser.C
pcvar_SOURCES			= pcvar.C
psrband_SOURCES			= psrband.C
psrsh_SOURCES			= psrsh.C
psrwt_SOURCES			= psrwt.C
pvt_SOURCES			= pvt.C
rmfit_SOURCES			= rmfit.C
bin_clean_SOURCES		= bin_clean.C
paf_SOURCES			= paf.C
psrqft_SOURCES			= psrqft.C
plot_baseline_SOURCES		= plot_baseline.C
pdmp_SOURCES                    = pdmp.C
psrfi_SOURCES                   = psrfi.C
spa_SOURCES                     = spa.C
tsys_SOURCES                    = tsys.C
paas_SOURCES			= paas.C
pfixbaseline_SOURCES		= pfixbaseline.C
psrchive_template_SOURCES       = psrchive_template.C

SimTiming_LDADD			= $(LDADD) @PGPLOT_LIBS@
binphs_test_LDADD		= $(LDADD) @PGPLOT_LIBS@
pacv_LDADD			= $(LDADD) @PGPLOT_LIBS@
pas_LDADD			= $(LDADD) @PGPLOT_LIBS@
pav_LDADD			= $(LDADD) @PGPLOT_LIBS@
pcm_LDADD			= $(LDADD) @PGPLOT_LIBS@ -lpthread
pcm_chooser_LDADD		= $(LDADD) @PGPLOT_LIBS@
pcvar_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrband_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrsh_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrwt_LDADD			= $(LDADD) @PGPLOT_LIBS@
pvt_LDADD			= $(LDADD) @PGPLOT_LIBS@
rmfit_LDADD			= $(LDADD) @PGPLOT_LIBS@
bin_clean_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrqft_LDADD			= $(LDADD) @PGPLOT_LIBS@
plot_baseline_LDADD		= $(LDADD) @PGPLOT_LIBS@
pdmp_LDADD		        = $(LDADD) @PGPLOT_LIBS@
psrfi_LDADD                     = $(LDADD) @PGPLOT_LIBS@
spa_LDADD                       = $(LDADD) @PGPLOT_LIBS@
tsys_LDADD                      = $(LDADD) @PGPLOT_LIBS@
paas_LDADD                      = $(LDADD) @PGPLOT_LIBS@
pfixbaseline_LDADD                      = $(LDADD) @PGPLOT_LIBS@
psrchive_template_LDADD         = $(LDADD) @PGPLOT_LIBS@

# future
# psrsh_LDADD = -lreadline -ltermcap

if HAVE_CFITSIO

bin_PROGRAMS += pad plot_acf

pad_SOURCES 			= pad.C
plot_acf_SOURCES		= plot_acf.C
pad_LDADD 			= $(LDADD) @PGPLOT_LIBS@
plot_acf_LDADD			= $(LDADD) @PGPLOT_LIBS@
endif

endif


##############################################################################
#
# Rule to install doxygen-generated documentation in $HOME/public_html/psrchive
#

EXPORT := ${HOME}/public_html/psrchive

export:
	doxygen
	rm -rf $(EXPORT)
	mv doc/html $(EXPORT)
	chmod -R a+r $(EXPORT)
	chmod a+x $(EXPORT)


#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/utils/include -I$(top_builddir)/astro/include \
        -I$(top_srcdir)/psrchive/Calibration \
	-I$(top_srcdir)/psrchive/Extension -I$(top_srcdir)/psrchive/timer \
	-I$(top_srcdir)/psrchive/epn -I$(top_srcdir)/psrchive/psrfits \
	-I$(top_srcdir)/psrchive/ascii \
	@PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @MPI_CFLAGS@

LDADD = libpsrchive.la $(top_builddir)/astro/libpsrastro.la \
        $(top_builddir)/utils/libpsrutils.la

libpsrchive_la_LIBADD += @FFT_LIBS@ @CFITSIO_LIBS@ @SLA_LIBS@ @FLIBS@

include $(top_srcdir)/config/Makefile.local

