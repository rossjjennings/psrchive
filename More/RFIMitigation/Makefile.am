
noinst_LTLIBRARIES = libRFIMitigation.la

nobase_include_HEADERS = \
	Pulsar/ChannelZapMedian.h \
	Pulsar/DoubleMedian.h \
	Pulsar/InterQuartileRange.h \
	Pulsar/LawnMower.h \
	Pulsar/Mower.h \
	Pulsar/RobustMower.h \
	Pulsar/SetThresholds.h \
	Pulsar/SumThreshold.h \
	Pulsar/TimeFrequencyMask.h \
	Pulsar/TimeFrequencySmooth.h \
	Pulsar/TimeFrequencyZap.h \
	Pulsar/TwoDeeMedian.h \
	Pulsar/ZapExtend.h \
	Pulsar/ZapInterpreter.h

libRFIMitigation_la_SOURCES = \
	ChannelZapMedian.C \
	DoubleMedian.C \
	InterQuartileRange.C \
	LawnMower.C \
	Mower.C \
	RobustMower.C \
	SetThresholds.C \
	SumThreshold.C \
	TimeFrequencyMask.C \
	TimeFrequencySmooth.C \
	TimeFrequencyZap.C \
	TwoDeeMedian.C \
	ZapExtend.C \
	ZapInterpreter.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += -I$(top_srcdir)/More/Resources

