
noinst_LTLIBRARIES = libCalibration.la

nobase_include_HEADERS = Calibration/CoherencyMeasurement.h
	Calibration/CoherencyMeasurementSet.h Calibration/ConvertMJD.h \
	Calibration/Faraday.h Calibration/Feed.h Calibration/Instrument.h \
	Calibration/MeanCoherency.h Calibration/MeanFeed.h \
	Calibration/MeanInstrument.h Calibration/MeanPolar.h \
	Calibration/MeanSingleAxis.h Calibration/MeasurementEquation.h \
	Calibration/Parallactic.h Calibration/ReceptionModel.h \
	Calibration/ReceptionModelAxisPlotter.h \
	Calibration/ReceptionModelPlotter.h Calibration/Simulation.h \
	Calibration/SingleAxis.h Calibration/SingleAxisPolynomial.h \
	Calibration/SingleAxisSolver.h

libCalibration_la_SOURCES = SingleAxis.C Feed.C Instrument.C		       \
	SingleAxisPolynomial.C Parallactic.C Faraday.C MeanCoherency.C	       \
	MeanPolar.C MeanSingleAxis.C MeanFeed.C MeanInstrument.C	       \
	MeasurementEquation.C ReceptionModel.C CoherencyMeasurement.C	       \
	CoherencyMeasurementSet.C ConvertMJD.C SingleAxisSolver.C Simulation.C \
	Polar_solve.C ReceptionModel_solve.C ReceptionModel_solve_thread.C

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
check_PROGRAMS = test_copy test_Feed test_SingleAxis \
	test_Parallactic test_ReceptionModel

test_Feed_SOURCES		= test_Feed.C
test_Parallactic_SOURCES	= test_Parallactic.C
test_ReceptionModel_SOURCES	= test_ReceptionModel.C
test_SingleAxis_SOURCES		= test_SingleAxis.C
test_copy_SOURCES		= test_copy.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_srcdir)/utils/include @PGPLOT_CFLAGS@

LDADD = libCalibration.la $(top_srcdir)/utils/libpsrutils.la

