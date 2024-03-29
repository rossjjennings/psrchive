
noinst_LTLIBRARIES = libgenutil.la

include_HEADERS = \
        Angle.h \
	Barycentre.h \
	BatchQueue.h \
        Brent.h \
        Cartesian.h \
	CommandLine.h \
        CommandParser.h \
        compressed_io.h \
	coord.h \
        coord_parse.h \
	Directional.h \
	DirectoryLock.h \
        dirutil.h \
	ExponentialDistribution.h \
	extutil.h \
	f77util.h \
	Fixed.h \
	format_it.h \
        fsleep.h \
	Horizon.h \
        ieee.h \
        ierf.h \
        JenetAnderson98A5.h \
        JenetAnderson98.h \
	KrausType.h \
	load_factory.h \
	LogFile.h \
        machine_endian.h \
	malloc16.h \
	Meridian.h \
        MJD.h \
	Mount.h \
	MountProjection.h \
	FITSUTC.h \
	NormalDistribution.h \
        orbital.h \
	pairutil.h \
	PhaseRange.h \
        Physical.h \
	Probability.h \
        Plugin.h \
	Ranges.h \
        RealTimer.h \
        RegularExpression.h \
	RobustEstimateZapper.h \
	RobustStats.h \
	RunningMedian.h \
        sky_coord.h \
	SmoothingSpline.h \
	StraightLine.h \
        strutil.h \
	SystemCall.h \
        templates.h \
	TemporaryDirectory.h \
	TemporaryFile.h \
	ThreadContext.h \
	ThreadMemory.h \
	ThreadStream.h \
        Types.h \
        typeutil.h \
        utc.h \
	VanVleck.h \
	VirtualMemory.h \
	table_stream.h \
	Warning.h


libgenutil_la_SOURCES = \
        Angle.C \
        angleconv.c \
	Barycentre.C \
	BatchQueue.C \
	BootstrapUncertainty.C \
	BoxMuller.C \
        Cartesian.C \
	CommandLine.C \
        CommandParser.C \
        companion_mass.c \
        completion.C \
        compressed_io.C \
        coord_parse.c \
	copy.C \
	CrossValidatedSmoothing.C \
	Directional.C \
	DirectoryLock.C \
        dirglob.C \
	dirglobtree.C \
        dirutil.c \
        eccentric_anomaly.c \
        endian.c \
        errscale.c \
	expand.C \
	ExponentialDistribution.C \
	f77util.c \
	filesize.c \
	Fixed.C \
	format_it.C \
        fsleep.c \
	Horizon.C \
        ieee.c \
        ierf.c \
        JenetAnderson98A5.C \
        JenetAnderson98.C \
	KrausType.C \
	LogFile.C \
	malloc16.c \
	Meridian.C \
        MJD.C \
	Mount.C \
	FITSUTC.C \
	NormalDistribution.C \
	PhaseRange.C \
        Physical.C \
	printable.C \
	Probability.C \
        Plugin.C \
	Ranges.C \
        RealTimer.C \
        RegularExpression.C \
	RobustEstimateZapper.C \
	replace_extension.C \
        sky_coord.C \
        sla_MJD.C \
        sla_sky_coord.C \
	smooth.f \
	SmoothingSpline.C \
        stringload.C \
        stringprintf.C \
        strutil.C \
	SystemCall.C \
	TemporaryDirectory.C \
	TemporaryFile.C \
	ThreadContext.C \
	ThreadMemory.C \
	ThreadStream.C \
	time_string.C \
        Types.C \
        utc.c \
	VanVleck.C \
	VirtualMemory.C \
	table_stream.C \
	Warning.C

if HAVE_GSL
include_HEADERS += PolynomialFit.h Interpolation.h dwt_undec.h SplineFit.h
libgenutil_la_SOURCES += PolynomialFit.C Interpolation.C dwt_undec.c SplineFit.C
endif

if HAVE_IPP
include_HEADERS += ThreadIPPMemory.h
libgenutil_la_SOURCES += ThreadIPPMemory.C
AM_CXXFLAGS = @IPP_CFLAGS@
endif

libgenutil_la_LIBADD = @READLINE_LIBS@ @GSL_LIBS@

bin_PROGRAMS = getMJD

getMJD_SOURCES = getMJD.C

TESTS = test_coord test_Cartesian test_MJD test_ja98	\
	test_endian test_sum test_NormalDistribution			\
	test_TemporaryFile test_moment2 test_MJD_ostream test_sky_coord	\
	test_exponential test_StraightLine test_ThreadStream		\
	test_Horizon test_LogFile test_Warning test_RunningMedian \
	test_PhaseRange 

check_PROGRAMS = $(TESTS) test_CommandLine test_CommandParser \
	test_Angle test_expand test_VirtualMemory

if HAVE_GSL
check_PROGRAMS += test_SplineFit
endif

test_Cartesian_SOURCES		= test_Cartesian.C
test_MJD_SOURCES		= test_MJD.C
test_Angle_SOURCES		= test_Angle.C
test_Angle_LDADD		= $(LDADD) @FLIBS@
test_CommandLine_SOURCES	= test_CommandLine.C
test_CommandParser_SOURCES      = test_CommandParser.C
test_CommandParser_LDADD	= $(LDADD) @READLINE_LIBS@
test_ja98_SOURCES		= test_ja98.C
test_sum_SOURCES		= test_sum.C
test_TemporaryFile_SOURCES	= test_TemporaryFile.C
test_moment2_SOURCES		= test_moment2.C
test_exponential_SOURCES	= test_exponential.C
test_MJD_ostream_SOURCES	= test_MJD_ostream.C
test_sky_coord_SOURCES		= test_sky_coord.C
test_expand_SOURCES		= test_expand.C
test_NormalDistribution_SOURCES = test_NormalDistribution.C
test_StraightLine_SOURCES	= test_StraightLine.C
test_SplineFit_SOURCES	        = test_SplineFit.C
test_ThreadStream_SOURCES	= test_ThreadStream.C
test_Horizon_SOURCES		= test_Horizon.C
test_VirtualMemory_SOURCES	= test_VirtualMemory.C
test_LogFile_SOURCES		= test_LogFile.C
test_Warning_SOURCES		= test_Warning.C
test_RunningMedian_SOURCES	= test_RunningMedian.C
test_PhaseRange_SOURCES		= test_PhaseRange.C

#############################################################################
#

LDADD = libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @GSL_CFLAGS@

