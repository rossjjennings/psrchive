
noinst_LTLIBRARIES = libExtension.la

nobase_include_HEADERS = \
	Pulsar/Backend.h                  Pulsar/IntegrationOrder.h  \
	Pulsar/BackendName.h              Pulsar/ObsExtension.h  \
	Pulsar/BinLngAscOrder.h           Pulsar/Parkes.h  \
	Pulsar/BinLngPeriOrder.h          Pulsar/Passband.h  \
	Pulsar/BinaryPhaseOrder.h         Pulsar/PeriastronOrder.h  \
	Pulsar/CalInfoExtension.h         Pulsar/Pointing.h  \
	Pulsar/CalibratorExtension.h      Pulsar/PolnCalibratorExtension.h  \
	Pulsar/CalibratorStokes.h         Pulsar/ProcHistory.h  \
	Pulsar/DigitiserStatistics.h      Pulsar/Receiver.h  \
	Pulsar/ExampleExtension.h         Pulsar/Telescope.h  \
	Pulsar/FITSHdrExtension.h         Pulsar/TwoBitStats.h  \
	Pulsar/FluxCalibratorExtension.h  Pulsar/WidebandCorrelator.h  \
	Pulsar/ITRFExtension.h            Pulsar/dspReduction.h

libExtension_la_SOURCES = \
	BackendName.C                    ObsExtension.C  \
	BinLngAscOrder.C                 Parkes.C  \
	BinLngPeriOrder.C                Passband.C  \
	BinaryPhaseOrder.C               PeriastronOrder.C  \
	CalInfoExtension.C               Pointing.C  \
	CalibratorExtension.C            PolnCalibratorExtension.C  \
	CalibratorExtension_build.C      PolnCalibratorExtension_build.C  \
	CalibratorStokes.C               ProcHistory.C  \
	DigitiserStatistics.C            Receiver.C  \
	ExampleExtension.C               Receiver_load.C  \
	FITSHdrExtension.C               Telescope.C  \
	FluxCalibratorExtension.C        TwoBitStats.C  \
	FluxCalibratorExtension_build.C  WidebandCorrelator.C  \
	ITRFExtension.C                  dspReduction.C  \
	IntegrationOrder.C

check_PROGRAMS = test_Receiver

test_Receiver_SOURCES = test_Receiver.C

