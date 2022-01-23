
noinst_LTLIBRARIES = libstat.la

nobase_include_HEADERS = BinaryStatistic.h UnaryStatistic.h evaluate.h \
	ChiSquared.h GeneralizedChiSquared.h

libstat_la_SOURCES = BinaryStatistic.C UnaryStatistic.C evaluate.C \
	ChiSquared.C GeneralizedChiSquared.C 

if HAVE_ARMADILLO
  nobase_include_HEADERS += GaussianMixtureProbabilityDensity.h
  libstat_la_SOURCES += GaussianMixtureProbabilityDensity.C
endif

TESTS = test_evaluate

check_PROGRAMS = $(TESTS)

test_evaluate_SOURCES = test_evaluate.C

#############################################################################
#

LDADD = libstat.la \
	$(top_builddir)/Util/fft/libpsrfft.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la \
	$(top_builddir)/Util/epsic/src/libepsic.la @FLIBS@

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @ARMADILLO_CFLAGS@

libstat_la_LIBADD = @ARMADILLO_LIBS@

