
noinst_LTLIBRARIES = libresources.la

nobase_include_HEADERS = Pulsar/SimplePredictor.h Pulsar/GeneratorInterpreter.h toa.h

libresources_la_SOURCES = Generator_default.C SimplePredictor.C \
	Parameters_children.C Predictor_children.C Site_location.C \
	GeneratorInterpreter.C Generator_factory.C \
	ParametersLookup_operator.C toa.C fit.C

bin_PROGRAMS = getHorizon toaio getRiseSet schedule benchmark_Predictor 

getHorizon_SOURCES = getHorizon.C
toaio_SOURCES = toaio.C
getRiseSet_SOURCES = getRiseSet.C
schedule_SOURCES = schedule.C
benchmark_Predictor_SOURCES = benchmark_Predictor.C

check_PROGRAMS = test_Parameters_factory test_Predictor_factory test_Predictor

test_Parameters_factory_SOURCES = test_Parameters_factory.C
test_Predictor_factory_SOURCES = test_Predictor_factory.C
test_Predictor_SOURCES = test_Predictor.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @TEMPO2_CFLAGS@

LDADD = libresources.la

if HAVE_TEMPO2
  LDADD += $(top_builddir)/Util/tempo2/libtempo_2.la
endif

LDADD += $(top_builddir)/Util/tempo/libtempo.la

LDADD += $(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la \
	$(top_builddir)/Util/epsic/src/libepsic.la @FLIBS@

