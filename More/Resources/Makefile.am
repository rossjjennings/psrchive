
noinst_LTLIBRARIES = libResources.la

libResources_la_SOURCES = \
	CommonOptions.C \
	StandardOptions.C \
  	standard_interface.C \
	standard_shell.C

nobase_include_HEADERS = \
	Pulsar/CommonOptions.h \
	Pulsar/StandardOptions.h


check_PROGRAMS = test_FrequencyMTM
test_FrequencyMTM_SOURCES = test_FrequencyMTM.C

##############################################################################
#
# yamlcpp-dependent code
#
if HAVE_YAMLCPP

  check_PROGRAMS += test_ConfigurableProjection
  test_ConfigurableProjection_SOURCES = test_ConfigurableProjection.C

endif

#############################################################################
#

LDADD = $(top_builddir)/More/libpsrmore.la \
        $(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la \
        $(top_builddir)/Util/meal/src/libMEAL.la \
        @YAMLCPP_LIBS@

include $(top_srcdir)/config/Makefile.include

