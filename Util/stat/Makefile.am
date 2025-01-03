
noinst_LTLIBRARIES = libstat.la

nobase_include_HEADERS = evaluate.h statutil.h \
	BinaryStatistic.h UnaryStatistic.h \
	ChiSquared.h GeneralizedChiSquared.h LinearRegression.h

libstat_la_SOURCES = evaluate.C statutil.C \
	BinaryStatistic.C UnaryStatistic.C \
	ChiSquared.C GeneralizedChiSquared.C LinearRegression.C

if HAVE_ARMADILLO
  nobase_include_HEADERS += GaussianMixtureProbabilityDensity.h
  libstat_la_SOURCES += GaussianMixtureProbabilityDensity.C
endif

TESTS = test_evaluate test_LinearRegression

check_PROGRAMS = $(TESTS)

test_evaluate_SOURCES = test_evaluate.C
test_LinearRegression_SOURCES = test_LinearRegression.C

#############################################################################
#

LDADD = libstat.la \
	$(top_builddir)/Util/fft/libpsrfft.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la \
	$(top_builddir)/Util/epsic/src/util/libutil.la @FLIBS@

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @ARMADILLO_CFLAGS@

libstat_la_LIBADD = @ARMADILLO_LIBS@

