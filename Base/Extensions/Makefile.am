
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
	Pulsar/DigitiserStatisticsTI.h \
	Pulsar/DigitiserCounts.h \
	Pulsar/DigitiserCountsTI.h \
        Pulsar/dspReduction.h \
        Pulsar/ExampleExtension.h \
        Pulsar/FeedExtension.h \
        Pulsar/FeedExtensionTI.h \
        Pulsar/FITSHdrExtension.h \
	Pulsar/FITSHdrExtensionTI.h \
	Pulsar/FITSSUBHdrExtension.h \
	Pulsar/FITSSUBHdrExtensionTI.h \
        Pulsar/FluxCalibratorExtension.h \
        Pulsar/FluxCalibratorExtensionTI.h \
        Pulsar/IntegrationOrder.h \
        Pulsar/ITRFExtension.h \
	Pulsar/ITRFExtensionTI.h \
        Pulsar/ObsExtension.h \
	Pulsar/ObsExtensionTI.h \
        Pulsar/Parkes.h \
        Pulsar/Passband.h \
        Pulsar/PassbandTI.h \
        Pulsar/PeriastronOrder.h \
        Pulsar/Pointing.h \
        Pulsar/PointingTI.h \
        Pulsar/PolnCalibratorExtension.h \
        Pulsar/PolnCalibratorExtensionTI.h \
        Pulsar/PowerEstimator.h \
        Pulsar/ProcHistory.h \
        Pulsar/ProcHistoryTI.h \
        Pulsar/Receiver_Field.h \
        Pulsar/Receiver.h \
        Pulsar/Receiver_Linear.h \
        Pulsar/Receiver_Native.h \
        Pulsar/ReceiverTI.h \
        Pulsar/TapeInfo.h \
        Pulsar/Telescope.h \
        Pulsar/TimeSortedOrder.h \
        Pulsar/TwoBitStats.h \
        Pulsar/WidebandCorrelator.h \
        Pulsar/WidebandCorrelatorTI.h \
	Pulsar/ExtensionRegister.h

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
	DigitiserStatisticsTI.C \
	DigitiserCounts.C \
	DigitiserCountsTI.C \
        dspReduction.C \
        ExampleExtension.C \
        FeedExtension.C \
        FeedExtensionTI.C \
        FITSHdrExtension.C \
	FITSHdrExtensionTI.C \
	FITSSUBHdrExtension.C \
	FITSSUBHdrExtensionTI.C \
        FluxCalibratorExtension_build.C \
        FluxCalibratorExtension.C \
        FluxCalibratorExtensionTI.C \
        IntegrationOrder.C \
        ITRFExtension.C \
        ITRFExtensionTI.C \
        ObsExtension.C \
	ObsExtensionTI.C \
        Parkes.C \
        Passband.C \
        PassbandTI.C \
        PeriastronOrder.C \
        Pointing.C \
        PointingTI.C \
        PolnCalibratorExtension.C \
        PolnCalibratorExtensionTI.C \
        ProcHistory.C \
        ProcHistoryTI.C \
        Receiver.C \
        ReceiverTI.C \
        Receiver_Field.C \
        Receiver_Linear.C \
        Receiver_Native.C \
        TapeInfo.C \
        Telescope.C \
        TimeSortedOrder.C \
        TwoBitStats.C \
        WidebandCorrelator.C \
        WidebandCorrelatorTI.C \
	ExtensionRegister.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include \
	-I$(top_srcdir)/More/Polarimetry

include $(top_srcdir)/config/Makefile.include

