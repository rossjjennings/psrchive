
noinst_LTLIBRARIES = libBases.la

nobase_include_HEADERS = \
	Pulsar/psrchive.h \
        Pulsar/ArchiveExpert.h \
        Pulsar/Archive.h \
        Pulsar/ArchiveTI.h \
        Pulsar/BasicArchive.h \
        Pulsar/BasicIntegration.h \
        Pulsar/Config.h \
        Pulsar/Container.h \
        Pulsar/ExampleArchive.h \
        Pulsar/IntegrationExpert.h \
        Pulsar/Integration.h \
        Pulsar/IntegrationManagerExpert.h \
        Pulsar/IntegrationManager.h \
        Pulsar/IntegrationTI.h \
        Pulsar/Profile.h \
        Pulsar/Pulsar.h

libBases_la_SOURCES = \
        Agent.C \
        Agent_plugin.C \
        Archive.C \
        Archive_copy.C \
        Archive_init_Integration.C \
        Archive_load.C \
        Archive_resize.C \
        ArchiveTI.C \
        Archive_unload.C \
        Archive_verbose.C \
        BasicArchive.C \
        BasicIntegration.C \
        Config.C \
        ExampleArchive.C \
        Integration.C \
        Integration_combine.C \
        IntegrationManager.C \
        Integration_mixable.C \
        Integration_resize.C \
        IntegrationTI.C \
        Profile_average.C \
        Profile.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/Core/Extensions

include $(top_srcdir)/config/Makefile.include

