
noinst_LTLIBRARIES = libBase.la

nobase_include_HEADERS = \
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

libBase_la_SOURCES = \
        Agent.C \
        Agent_plugin.C \
        Agent_static.C \
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

INCLUDES = -I$(top_builddir)/utils/include \
	-I$(top_srcdir)/psrchive/File/Extension \
	-I$(top_srcdir)/psrchive/File/Timer \
	-I$(top_srcdir)/psrchive/File/PSRFITS \
	-I$(top_srcdir)/psrchive/File/EPN \
	-I$(top_srcdir)/psrchive/File/ASCII

