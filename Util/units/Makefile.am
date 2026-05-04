
noinst_LTLIBRARIES = libunits.la

include_HEADERS = \
	Cloude.h \
	Configuration.h \
	escape.h \
	EstimateStats.h \
	file_cast.h \
	FilePtr.h \
	identifiable_factory.h \
	Identifiable.h \
	lazy.h \
	MethodFunction.h \
	ndArray.h \
	ndIndex.h \
	OwnStream.h \
	Registry.h \
	stringcase.h \
	substitute.h

libunits_la_SOURCES = \
	Configuration.C \
	Estimate.C \
	escape.C \
	FilePtr.C \
	Identifiable.C \
	OwnStream.C \
	stringcase.C \
	test.cfg 

TESTS = test_ndIndex test_ndArray \
	test_Configuration test_Registry \
	test_substitute test_escape test_MethodFunction 

check_PROGRAMS = $(TESTS) 

test_ndIndex_SOURCES		= test_ndIndex.C
test_ndArray_SOURCES		= test_ndArray.C
test_Configuration_SOURCES	= test_Configuration.C
test_Registry_SOURCES		= test_Registry.C
test_substitute_SOURCES		= test_substitute.C
test_escape_SOURCES		= test_escape.C

test_MethodFunction_SOURCES	= test_MethodFunction.C

#############################################################################
#

LDADD = libunits.la \
        $(top_builddir)/Util/meal/src/util/libutil.la \
        $(top_builddir)/Util/epsic/src/util/libutil.la

include $(top_srcdir)/config/Makefile.include
