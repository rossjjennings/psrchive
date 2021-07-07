
noinst_LTLIBRARIES = libstat.la

nobase_include_HEADERS = BinaryStatistic.h UnaryStatistic.h evaluate.h

libstat_la_SOURCES = BinaryStatistic.C UnaryStatistic.C evaluate.C

TESTS = test_evaluate

test_evaluate_SOURCES = test_evaluate.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

LDADD = libstat.la

LDADD += $(top_builddir)/Util/fft/libpsrfft.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la \
	$(top_builddir)/Util/epsic/src/libepsic.la @FLIBS@

