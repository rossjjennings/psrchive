
lib_LTLIBRARIES = libpsrchive.la

nobase_include_HEADERS = \
	Pulsar/AdaptiveSNR.h             Pulsar/PolarCalibrator.h \
	Pulsar/Archive.h                 Pulsar/PolnCalibrator.h \
	Pulsar/ArchiveProcessor.h        Pulsar/PolnProfile.h \
	Pulsar/BaselineFunction.h        Pulsar/PolnProfileFit.h \
	Pulsar/BaselineMask.h            Pulsar/Profile.h \
	Pulsar/BasicArchive.h            Pulsar/PulsarCalibrator.h \
	Pulsar/BasicIntegration.h        Pulsar/RFIMitigation.h \
	Pulsar/Calibrator.h              Pulsar/ReceptionCalibrator.h \
	Pulsar/CalibratorPlotter.h       Pulsar/ReceptionCalibratorPlotter.h \
	Pulsar/CalibratorStokesInfo.h    Pulsar/ReferenceCalibrator.h \
	Pulsar/CorrectionsCalibrator.h   Pulsar/SingleAxisCalibrator.h \
	Pulsar/Database.h                Pulsar/Smooth.h \
	Pulsar/ExampleArchive.h          Pulsar/SmoothMean.h \
	Pulsar/FluxCalibrator.h          Pulsar/SmoothMedian.h \
	Pulsar/FluxCalibratorDatabase.h  Pulsar/SourceInfo.h \
	Pulsar/FourierSNR.h              Pulsar/StandardSNR.h \
	Pulsar/HybridCalibrator.h        Pulsar/SyntheticProfile.h \
	Pulsar/InstrumentInfo.h          Pulsar/SystemCalibrator.h \
	Pulsar/Integration.h             Pulsar/Transposer.h \
	Pulsar/IntegrationManager.h      Pulsar/Utilities.h \
	Pulsar/PhaseWeight.h             Pulsar/ArchiveTI.h \
	Pulsar/PhaseWeightFunction.h     model_profile.h \
	Pulsar/Plotter.h                 Pulsar/IntegrationTI.h \
	Pulsar/ExtensionTI.h

libpsrchive_la_SOURCES = plugins.csh getopt.h Physical.h plotJones.C \
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
	Archive_resize.C              Profile.C  \
	Archive_split_method.C        Profile_average.C  \
	Archive_toas.C                Profile_basic_shift.C  \
	Archive_total.C               Profile_convolve.C  \
	Archive_transform.C           Profile_denoise.C  \
	Archive_tscrunch.C            Profile_find_peak_edges.C  \
	Archive_unload.C              Profile_find_transitions.C  \
	Archive_update_model.C        Profile_flux.C  \
	Archive_verbose.C             Profile_rotate.C  \
	BaselineMask.C                Profile_shift.C  \
	BasicArchive.C                Profile_snr.C  \
	BasicIntegration.C            Profile_snr_fortran.C  \
	Calibrator.C                  Profile_stats.C  \
	CalibratorStokesInfo.C        Profile_toa.C  \
	CorrectionsCalibrator.C       PulsarCalibrator.C  \
	Database.C                    RFIMitigation.C  \
	ExampleArchive.C              ReceptionCalibrator.C  \
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
	Integration_defaraday.C       SystemCalibrator.C  \
	Integration_fappend.C         Transposer.C  \
	Integration_find.C            Utilities.C  \
	Integration_flux.C            choose.C  \
	Integration_fscrunch.C        getopt.c  \
	Integration_get_PA.C          getopt1.c  \
	Integration_get_Stokes.C      model_profile.C  \
	Integration_invint.C          pafit_get_PA.C  \
	Integration_match.C           ArchiveTI.C \
	IntegrationTI.C               ArchiveTI_import.C \
	PhaseGradShift.C

SUBDIRS                 = Calibration Extension timer epn

libpsrchive_la_LIBADD   = Calibration/libCalibration.la \
			Extension/libExtension.la \
			timer/libtimer.la epn/libepn.la

if HAVE_CFITSIO

SUBDIRS += psrfits
libpsrchive_la_LIBADD += psrfits/libpsrfits.la

endif

SUBDIRS += .

if HAVE_QT

SUBDIRS += GUI

endif


# ######################################################################
#
# programs
#
bin_PROGRAMS = pac vap vip psredit psradd fadd pat pam fluxcal \
	fix_coords test_Difference fix_hydra sesdiv \
        fix_3C353 archive_correct archive_denoise \
        fix_fluxcal archive_split psrconv pascii \
	default_loader

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

pac_SOURCES			= pac.C
pac_LDADD			= $(LDADD) @SLA_LIBS@

pam_SOURCES			= pam.C
pascii_SOURCES			= pascii.C
pat_SOURCES			= pat.C
psradd_SOURCES			= psradd.C
psrconv_SOURCES			= psrconv.C
sesdiv_SOURCES			= sesdiv.C
vap_SOURCES			= vap.C
vip_SOURCES			= vip.C
psredit_SOURCES			= psredit.C

check_PROGRAMS = test_Archive test_denoise test_Difference

test_Archive_SOURCES		= test_Archive.C
test_denoise_SOURCES		= test_denoise.C
test_Difference_SOURCES		= test_Difference.C


# ######################################################################
#
# PGPLOT specific code
#
if HAVE_PGPLOT

libpsrchive_la_SOURCES += ArchiveProcessor.C Plotter.C CalibratorPlotter.C  \
	ReceptionCalibratorPlotter.C  \
	Plotter_Mercator.C             Plotter_phase_subints.C  \
	Plotter_baseline_spectrum.C    Plotter_plot.C  \
	Plotter_calibrator_spectrum.C  Plotter_set_colour_map.C  \
	Plotter_draw_transitions.C     Plotter_simple_ds.C  \
	Plotter_phase_fourier.C        Plotter_spherical.C  \
	Plotter_phase_frequency.C

bin_PROGRAMS += pacv pcvar paz pav pas psrwt psrsh pcm pvt \
	binphs_test pcm_chooser rmfit psrband SimTiming

SimTiming_SOURCES		= SimTiming.C
binphs_test_SOURCES		= binphs_test.C
pacv_SOURCES			= pacv.C
pas_SOURCES			= pas.C
pav_SOURCES			= pav.C
paz_SOURCES			= paz.C
pcm_SOURCES			= pcm.C
pcm_chooser_SOURCES		= pcm_chooser.C
pcvar_SOURCES			= pcvar.C
psrband_SOURCES			= psrband.C
psrsh_SOURCES			= psrsh.C
psrwt_SOURCES			= psrwt.C
pvt_SOURCES			= pvt.C
rmfit_SOURCES			= rmfit.C

SimTiming_LDADD			= $(LDADD) @PGPLOT_LIBS@
binphs_test_LDADD		= $(LDADD) @PGPLOT_LIBS@
pacv_LDADD			= $(LDADD) @PGPLOT_LIBS@
pas_LDADD			= $(LDADD) @PGPLOT_LIBS@
pav_LDADD			= $(LDADD) @PGPLOT_LIBS@
paz_LDADD			= $(LDADD) @PGPLOT_LIBS@
pcm_LDADD			= $(LDADD) @PGPLOT_LIBS@ -lpthread
pcm_chooser_LDADD		= $(LDADD) @PGPLOT_LIBS@
pcvar_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrband_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrsh_LDADD			= $(LDADD) @PGPLOT_LIBS@
psrwt_LDADD			= $(LDADD) @PGPLOT_LIBS@
pvt_LDADD			= $(LDADD) @PGPLOT_LIBS@
rmfit_LDADD			= $(LDADD) @PGPLOT_LIBS@

# future
# psrsh_LDADD = -lreadline -ltermcap

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
	cp documentation/build/autotools.* $(EXPORT)
	cp documentation/build/*.html $(EXPORT)
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
	@PGPLOT_CFLAGS@

LDADD = libpsrchive.la $(top_builddir)/astro/libpsrastro.la \
        $(top_builddir)/utils/libpsrutils.la \
	@FFT_LIBS@ @CFITSIO_LIBS@ @SLA_LIBS@

include $(top_srcdir)/config/Makefile.local

