
noinst_LTLIBRARIES = libPolarimetry.la

nobase_include_HEADERS = Pulsar/CoherencyMeasurement.h \
	Pulsar/CoherencyMeasurementSet.h Pulsar/ConvertMJD.h \
	Pulsar/Faraday.h Pulsar/IRIonosphere.h \
	Pulsar/Feed.h Pulsar/Instrument.h \
	Pulsar/MeanCoherency.h Pulsar/MeanFeed.h \
	Pulsar/MeanInstrument.h Pulsar/MeanPolar.h \
	Pulsar/MeanSingleAxis.h Pulsar/MeasurementEquation.h \
	Pulsar/Parallactic.h Pulsar/ReceptionModel.h \
	Pulsar/ReceptionModelAxisPlotter.h \
	Pulsar/ReceptionModelPlotter.h Pulsar/Simulation.h \
	Pulsar/SingleAxis.h Pulsar/SingleAxisPolynomial.h \
	Pulsar/SingleAxisSolver.h Pulsar/StandardModel.h \
	Pulsar/ObservationUncertainty.h \
	Pulsar/TemplateUncertainty.h \
	Pulsar/TotalCovariance.h

libPolarimetry_la_SOURCES = SingleAxis.C Feed.C Instrument.C		   \
	SingleAxisPolynomial.C Parallactic.C Faraday.C IRIonosphere.C	   \
	MeanCoherency.C MeanPolar.C MeanSingleAxis.C MeanFeed.C		   \
	MeanInstrument.C MeasurementEquation.C ReceptionModel.C		   \
	CoherencyMeasurement.C CoherencyMeasurementSet.C ConvertMJD.C	   \
	SingleAxisSolver.C Simulation.C StandardModel.C Feed_load.C	   \
	Polar_solve.C ReceptionModel_solve.C ReceptionModel_solve_thread.C \
	ObservationUncertainty.C TemplateUncertainty.C TotalCovariance.C

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

INCLUDES = -I$(top_builddir)/utils/include -I$(top_srcdir)/psrchive \
	@PGPLOT_CFLAGS@ @SLA_CFLAGS@

LDADD = libPolarimetry.la $(top_builddir)/More/MEAL/libmeal.la \
	$(top_builddir)/utils/libpsrutils.la

