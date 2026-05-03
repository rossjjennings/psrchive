
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
	interface_stream.h \
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
	test_Configuration test_Registry test_Functor \
	test_Callback test_DynamicInterface \
	test_substitute test_escape test_ModifyRestore test_MethodFunction 

check_PROGRAMS = $(TESTS) many_references many_calls benchmark_tostring examples_Reference

test_ndIndex_SOURCES		= test_ndIndex.C
test_ndArray_SOURCES		= test_ndArray.C
test_Configuration_SOURCES	= test_Configuration.C
test_Registry_SOURCES		= test_Registry.C
test_Functor_SOURCES		= test_Functor.C
test_Callback_SOURCES		= test_Callback.C
test_substitute_SOURCES		= test_substitute.C
test_escape_SOURCES		= test_escape.C

test_DynamicInterface_SOURCES	= test_DynamicInterface.C

test_ModifyRestore_SOURCES      = test_ModifyRestore.C
test_MethodFunction_SOURCES	= test_MethodFunction.C

many_references_SOURCES		= many_references.C
many_calls_SOURCES		= many_calls.C

examples_Reference_SOURCES	= examples_Reference.C

#############################################################################
#

LDADD = libunits.la

include $(top_srcdir)/config/Makefile.include
