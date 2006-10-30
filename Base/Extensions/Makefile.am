
noinst_LTLIBRARIES = libExtensions.la

nobase_include_HEADERS = \
        Pulsar/Backend.h \
        Pulsar/BackendTI.h \
        Pulsar/BinaryPhaseOrder.h \
        Pulsar/BinLngAscOrder.h \
        Pulsar/BinLngPeriOrder.h \
        Pulsar/CalibratorExtension.h \
        Pulsar/CalibratorStokes.h \
        Pulsar/CalInfoExtension.h \
        Pulsar/ColdPlasmaHistory.h \
        Pulsar/Dedisperse.h \
        Pulsar/DeFaraday.h \
        Pulsar/DigitiserStatistics.h \
        Pulsar/dspReduction.h \
        Pulsar/ExampleExtension.h \
        Pulsar/FeedExtension.h \
        Pulsar/FITSHdrExtension.h \
        Pulsar/FluxCalibratorExtension.h \
        Pulsar/IntegrationOrder.h \
        Pulsar/ITRFExtension.h \
        Pulsar/ObsExtension.h \
        Pulsar/Parkes.h \
        Pulsar/Passband.h \
        Pulsar/PeriastronOrder.h \
        Pulsar/Pointing.h \
        Pulsar/PointingTI.h \
        Pulsar/PolnCalibratorExtension.h \
        Pulsar/PowerEstimator.h \
        Pulsar/ProcHistory.h \
        Pulsar/Receiver_Field.h \
        Pulsar/Receiver.h \
        Pulsar/Receiver_Linear.h \
        Pulsar/Receiver_Native.h \
        Pulsar/ReceiverTI.h \
        Pulsar/TapeInfo.h \
        Pulsar/Telescope.h \
        Pulsar/TimeSortedOrder.h \
        Pulsar/TwoBitStats.h \
        Pulsar/WidebandCorrelator.h

libExtensions_la_SOURCES = \
        Backend.C \
        BackendTI.C \
        BinaryPhaseOrder.C \
        BinLngAscOrder.C \
        BinLngPeriOrder.C \
        CalibratorExtension_build.C \
        CalibratorExtension.C \
        CalibratorStokes.C \
        CalInfoExtension.C \
        ColdPlasmaHistory.C \
        Dedisperse.C \
        DeFaraday.C \
        DigitiserStatistics.C \
        dspReduction.C \
        ExampleExtension.C \
        FeedExtension.C \
        FITSHdrExtension.C \
        FluxCalibratorExtension_build.C \
        FluxCalibratorExtension.C \
        IntegrationOrder.C \
        ITRFExtension.C \
        ObsExtension.C \
        Parkes.C \
        Passband.C \
        PeriastronOrder.C \
        Pointing.C \
        PointingTI.C \
        PolnCalibratorExtension.C \
        ProcHistory.C \
        Receiver.C \
        Receiver_Field.C \
        Receiver_Linear.C \
        Receiver_Native.C \
        ReceiverTI.C \
        TapeInfo.C \
        Telescope.C \
        TimeSortedOrder.C \
        TwoBitStats.C \
        WidebandCorrelator.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include \
	-I$(top_srcdir)/More/Polarimetry

include $(top_srcdir)/config/Makefile.include

