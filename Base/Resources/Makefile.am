
noinst_LTLIBRARIES = libResources.la

libResources_la_SOURCES = Generator_default.C \
	Parameters_children.C Predictor_children.C Site_location.C

bin_PROGRAMS = getHorizon

check_PROGRAMS = test_Parameters_factory test_Predictor_factory

getHorizon_SOURCES = getHorizon.C
test_Parameters_factory_SOURCES = test_Parameters_factory.C
test_Predictor_factory_SOURCES = test_Predictor_factory.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.local

AM_CPPFLAGS = -I$(top_builddir)/local_include @TEMPO2_CFLAGS@

LDADD = libResources.la $(top_builddir)/Util/libpsrutil.la

