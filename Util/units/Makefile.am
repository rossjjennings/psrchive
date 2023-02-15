
noinst_LTLIBRARIES = libunits.la

include_HEADERS = \
	Alias.h \
        Callback.h \
        Cloude.h \
	complex_public.h \
        Configuration.h \
	debug.h \
        Error.h \
	escape.h \
        EstimateStats.h \
	Factory.h \
	file_cast.h \
	FilePtr.h \
        Functor.h \
        HeapTracked.h \
	identifiable_factory.h \
	Identifiable.h \
	interface_factory.h \
	interface_stream.h \
	lazy.h \
	MethodFunction.h \
        ModifyRestore.h \
	NestedTextInterface.h \
	ndArray.h \
	ndIndex.h \
	OwnStream.h \
        pad.h \
        ReferenceAble.h \
        Reference.h \
        ReferenceTo.h \
	ReferenceVector.h \
        Registry.h \
        separate.h \
	Separator.h \
	stringcase.h \
        stringtok.h \
        substitute.h \
	TextEditor.h \
	TextIndex.h \
        TextInterface.h \
	TextInterfaceAdapter.h \
	TextInterfaceAttribute.h \
	TextInterfaceElement.h \
	TextInterfaceEmbed.h \
	TextInterfaceEstimate.h \
	TextInterfaceFunction.h \
	TextInterfaceInterpreter.h \
	TextInterfaceName.h \
	TextInterfaceParser.h \
	TextInterfaceProxy.h \
	TextInterfaceTo.h \
	TextInterfaceValue.h \
	TextLoop.h \
        tostring.h \
        Traits.h

libunits_la_SOURCES = \
	Alias.C \
        Configuration.C \
        Error.C \
	Estimate.C \
	escape.C \
	FilePtr.C \
        HeapTracked.C \
	Identifiable.C \
	OwnStream.C \
        pad.C \
	parse_indeces.C \
        ReferenceAble.C \
        separate.C \
	Separator.C \
	stringcase.C \
        stringtok.C \
	test.cfg \
	tostring.C \
	TextIndex.C \
        TextInterface.C \
	TextInterfaceName.C \
	TextLoop.C

TESTS = test_ndIndex test_ndArray test_Alias test_tostring test_Error	\
	test_Configuration test_HeapTracked test_Reference		\
	test_circular_Reference test_Registry test_Functor		\
	test_Callback test_separate test_TextInterface			\
	test_parse_indeces test_TextIndex test_DynamicInterface		\
	test_substitute test_escape test_ModifyRestore test_MethodFunction

check_PROGRAMS = $(TESTS) many_references many_calls benchmark_tostring

test_ndIndex_SOURCES		= test_ndIndex.C
test_ndArray_SOURCES		= test_ndArray.C
test_Alias_SOURCES		= test_Alias.C
test_tostring_SOURCES		= test_tostring.C
test_Error_SOURCES		= test_Error.C
test_Configuration_SOURCES	= test_Configuration.C
test_HeapTracked_SOURCES	= test_HeapTracked.C
test_Reference_SOURCES		= test_Reference.C
test_circular_Reference_SOURCES	= test_circular_Reference.C
test_Registry_SOURCES		= test_Registry.C
test_Functor_SOURCES		= test_Functor.C
test_Callback_SOURCES		= test_Callback.C
test_separate_SOURCES		= test_separate.C
test_substitute_SOURCES		= test_substitute.C
test_escape_SOURCES		= test_escape.C

test_TextInterface_SOURCES	= test_TextInterface.C
test_DynamicInterface_SOURCES	= test_DynamicInterface.C
test_parse_indeces_SOURCES	= test_parse_indeces.C
test_TextIndex_SOURCES		= test_TextIndex.C

test_ModifyRestore_SOURCES      = test_ModifyRestore.C
test_MethodFunction_SOURCES	= test_MethodFunction.C

many_references_SOURCES		= many_references.C
many_calls_SOURCES		= many_calls.C

benchmark_tostring_SOURCES	= benchmark_tostring.C

#############################################################################
#

LDADD = libunits.la

include $(top_srcdir)/config/Makefile.include
