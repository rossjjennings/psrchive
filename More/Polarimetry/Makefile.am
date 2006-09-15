
noinst_LTLIBRARIES = libCalibration.la

nobase_include_HEADERS = Calibration/CoherencyMeasurement.h \
	Calibration/CoherencyMeasurementSet.h Calibration/ConvertMJD.h \
	Calibration/Faraday.h Calibration/IRIonosphere.h \
	Calibration/Feed.h Calibration/Instrument.h \
	Calibration/MeanCoherency.h Calibration/MeanFeed.h \
	Calibration/MeanInstrument.h Calibration/MeanPolar.h \
	Calibration/MeanSingleAxis.h Calibration/MeasurementEquation.h \
	Calibration/Parallactic.h Calibration/ReceptionModel.h \
	Calibration/ReceptionModelAxisPlotter.h \
	Calibration/ReceptionModelPlotter.h Calibration/Simulation.h \
	Calibration/SingleAxis.h Calibration/SingleAxisPolynomial.h \
	Calibration/SingleAxisSolver.h Calibration/StandardModel.h \
	Calibration/ObservationUncertainty.h \
	Calibration/TemplateUncertainty.h \
	Calibration/TotalCovariance.h

libCalibration_la_SOURCES = SingleAxis.C Feed.C Instrument.C		   \
	SingleAxisPolynomial.C Parallactic.C Faraday.C IRIonosphere.C	   \
	MeanCoherency.C MeanPolar.C MeanSingleAxis.C MeanFeed.C		   \
	MeanInstrument.C MeasurementEquation.C ReceptionModel.C		   \
	CoherencyMeasurement.C CoherencyMeasurementSet.C ConvertMJD.C	   \
	SingleAxisSolver.C Simulation.C StandardModel.C Feed_load.C	   \
	Polar_solve.C ReceptionModel_solve.C ReceptionModel_solve_thread.C \
	ObservationUncertainty.C TemplateUncertainty.C TotalCovariance.C

SUBDIRS = iri2001

libCalibration_la_LIBADD = iri2001/libiri.la

bin_PROGRAMS = jones_entropy rmrot

jones_entropy_SOURCES	= jones_entropy.C
rmrot_SOURCES		= rmrot.C

##############################################################################
#
# PGPLOT-specific code
#
if HAVE_PGPLOT

libCalibration_la_SOURCES += ReceptionModelPlotter.C

bin_PROGRAMS += calsim pafit

calsim_SOURCES		= calsim.C
calsim_LDADD		= $(LDADD) @PGPLOT_LIBS@

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

INCLUDES = -I$(top_builddir)/utils/include -I$(top_srcdir)/psrchive \
	@PGPLOT_CFLAGS@ @SLA_CFLAGS@

LDADD = libCalibration.la $(top_builddir)/utils/libpsrutils.la

