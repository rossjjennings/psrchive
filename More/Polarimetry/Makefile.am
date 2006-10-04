
noinst_LTLIBRARIES = libPolarimetry.la

nobase_include_HEADERS = \
        Pulsar/Calibrator.h \
        Pulsar/CalibratorPlotter.h \
        Pulsar/CalibratorStokesInfo.h \
        Pulsar/CoherencyMeasurement.h \
        Pulsar/CoherencyMeasurementSet.h \
        Pulsar/ConvertMJD.h \
        Pulsar/CorrectionsCalibrator.h \
        Pulsar/Database.h \
        Pulsar/DeltaPA.h \
        Pulsar/DeltaRM.h \
        Pulsar/DoPCalibrator.h \
        Pulsar/Faraday.h \
        Pulsar/FaradayRotation.h \
        Pulsar/Feed.h \
        Pulsar/FluxCalibratorData.h \
        Pulsar/FluxCalibrator.h \
        Pulsar/HybridCalibrator.h \
        Pulsar/Instrument.h \
        Pulsar/IonosphereCalibrator.h \
        Pulsar/IRIonosphere.h \
        Pulsar/MeanCoherency.h \
        Pulsar/MeanFeed.h \
        Pulsar/MeanInstrument.h \
        Pulsar/MeanPolar.h \
        Pulsar/MeanSingleAxis.h \
        Pulsar/MeasurementEquation.h \
        Pulsar/ObservationUncertainty.h \
        Pulsar/OffPulseCalibrator.h \
        Pulsar/Parallactic.h \
        Pulsar/PolarCalibrator.h \
        Pulsar/PolnCalibrator.h \
        Pulsar/PulsarCalibrator.h \
        Pulsar/ReceptionCalibrator.h \
        Pulsar/ReceptionCalibratorPlotter.h \
        Pulsar/ReceptionModelAxisPlotter.h \
        Pulsar/ReceptionModel.h \
        Pulsar/ReceptionModelPlotter.h \
        Pulsar/ReferenceCalibrator.h \
        Pulsar/ReflectStokes.h \
        Pulsar/Simulation.h \
        Pulsar/SingleAxisCalibrator.h \
        Pulsar/SingleAxis.h \
        Pulsar/SingleAxisPolynomial.h \
        Pulsar/SingleAxisSolver.h \
        Pulsar/StandardModel.h \
        Pulsar/SystemCalibrator.h \
        Pulsar/TemplateUncertainty.h \
        Pulsar/TotalCovariance.h

libPolarimetry_la_SOURCES = \
        Calibrator.C \
        CalibratorPlotter.C \
        CalibratorStokesInfo.C \
        choose.C \
        CoherencyMeasurement.C \
        CoherencyMeasurementSet.C \
        ConvertMJD.C \
        CorrectionsCalibrator.C \
        Database.C \
        DeltaPA.C \
        DeltaRM.C \
        DoPCalibrator.C \
        Faraday.C \
        FaradayRotation.C \
        Feed.C \
        Feed_load.C \
        FluxCalibrator.C \
        FluxCalibratorData.C \
        HybridCalibrator.C \
        Instrument.C \
        InstrumentInfo.C \
        Integration_defaraday.C \
        IonosphereCalibrator.C \
        IRIonosphere.C \
        MeanCoherency.C \
        MeanFeed.C \
        MeanInstrument.C \
        MeanPolar.C \
        MeanSingleAxis.C \
        MeasurementEquation.C \
        ObservationUncertainty.C \
        OffPulseCalibrator.C \
        Parallactic.C \
        PolarCalibrator.C \
        Polarization.C \
        Polar_solve.C \
        PolnCalibrator.C \
        PolnCalibratorExt.C \
        PolnCalibratorInfo.C \
        PolnProfile.C \
        PolnProfileFitAnalysis.C \
        PolnProfileFit.C \
        PolnProfileStats.C \
        PulsarCalibrator.C \
        Receiver_get_transformation.C \
        Receiver_load.C \
        ReceptionCalibrator.C \
        ReceptionCalibrator_get_Info.C \
        ReceptionCalibrator_get_solution.C \
        ReceptionCalibratorPlotter.C \
        ReceptionModel.C \
        ReceptionModelPlotter.C \
        ReceptionModel_solve.C \
        ReceptionModel_solve_thread.C \
        ReferenceCalibrator.C \
        ReflectStokes.C \
        Simulation.C \
        SingleAxis.C \
        SingleAxisCalibrator.C \
        SingleAxisPolynomial.C \
        SingleAxisSolver.C \
        SourceInfo.C \
        StandardModel.C \
        SystemCalibrator.C \
        TemplateUncertainty.C \
        TotalCovariance.C

SUBDIRS = iri2001

libPolarimetry_la_LIBADD = iri2001/libiri.la

bin_PROGRAMS = rmrot

rmrot_SOURCES = rmrot.C

##############################################################################
#
# PGPLOT-specific code
#
if HAVE_PGPLOT

libPolarimetry_la_SOURCES += ReceptionModelPlotter.C

bin_PROGRAMS += pafit

pafit_SOURCES		= pafit.C 
pafit_LDADD		= $(LDADD) @PGPLOT_LIBS@ @SLA_LIBS@

endif

# ######################################################################
#
# programs
#

TESTS = test_copy test_Feed test_SingleAxis test_TotalCovariance \
	test_Parallactic test_ReceptionModel

check_PROGRAMS = $(TESTS) test_IRIonosphere

test_Feed_SOURCES		= test_Feed.C
test_Parallactic_SOURCES	= test_Parallactic.C
test_ReceptionModel_SOURCES	= test_ReceptionModel.C
test_SingleAxis_SOURCES		= test_SingleAxis.C
test_TotalCovariance_SOURCES	= test_TotalCovariance.C

test_copy_SOURCES		= test_copy.C
test_IRIonosphere_SOURCES	= test_IRIonosphere.C

test_Parallactic_LDADD		= $(LDADD) @SLA_LIBS@
test_ReceptionModel_LDADD	= $(LDADD) @SLA_LIBS@

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include @PGPLOT_CFLAGS@ @SLA_CFLAGS@

LDADD = libPolarimetry.la $(top_builddir)/More/MEAL/libMEAL.la \
	$(top_builddir)/utils/libpsrutils.la

include $(top_srcdir)/config/Makefile.include

