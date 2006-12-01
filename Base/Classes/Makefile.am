
noinst_LTLIBRARIES = libClasses.la

nobase_include_HEADERS = \
	Pulsar/psrchive.h \
	Pulsar/Agent.h \
        Pulsar/ArchiveExpert.h \
        Pulsar/Archive.h \
        Pulsar/ArchiveTI.h \
        Pulsar/BasicArchive.h \
        Pulsar/BasicIntegration.h \
	Pulsar/Check.h \
        Pulsar/Config.h \
        Pulsar/Container.h \
	Pulsar/Correction.h \
        Pulsar/ExampleArchive.h \
	Pulsar/FITSAlias.h \
        Pulsar/IntegrationExpert.h \
        Pulsar/Integration.h \
        Pulsar/IntegrationManagerExpert.h \
        Pulsar/IntegrationManager.h \
        Pulsar/IntegrationTI.h \
        Pulsar/Profile.h \
        Pulsar/Pulsar.h \
	Pulsar/Verification.h

libClasses_la_SOURCES = \
        Agent.C \
        Agent_plugin.C \
        Archive.C \
	Archive_correct.C \
        Archive_copy.C \
        Archive_init_Integration.C \
        Archive_load.C \
        Archive_resize.C \
	Archive_verify.C \
        ArchiveTI.C \
        Archive_unload.C \
        Archive_verbose.C \
        BasicArchive.C \
        BasicIntegration.C \
	Check.C \
        Config.C \
        ExampleArchive.C \
	FITSAlias.C \
        Integration.C \
        Integration_combine.C \
        IntegrationManager.C \
        Integration_mixable.C \
        Integration_resize.C \
        IntegrationTI.C \
        Profile_average.C \
        Profile.C \
	Pulsar.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/Base/Extensions

include $(top_srcdir)/config/Makefile.include

