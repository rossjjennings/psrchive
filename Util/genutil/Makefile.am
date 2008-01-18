
noinst_LTLIBRARIES = libgenutil.la

include_HEADERS = \
        Angle.h \
	BatchQueue.h \
        Brent.h \
	BoxMuller.h \
        Cartesian.h \
        CommandParser.h \
        compressed_io.h \
        coord.h \
        coord_parse.h \
        dirutil.h \
        environ.h \
	evaluate.h \
	ExponentialDistribution.h \
	factory.h \
        fcomp.h \
	format_it.h \
        fsleep.h \
	Horizon.h \
        ieee.h \
        ierf.h \
        JenetAnderson98A5.h \
        JenetAnderson98.h \
        machine_endian.h \
        MJD.h \
	FITSUTC.h \
	NormalDistribution.h \
        orbital.h \
	pairutil.h \
        Physical.h \
	Probability.h \
        Plugin.h \
        RealTimer.h \
        RegularExpression.h \
        sky_coord.h \
	StraightLine.h \
        strutil.h \
        templates.h \
	TemporaryFile.h \
	ThreadContext.h \
        Types.h \
        typeutil.h \
        utc.h \
	VirtualMemory.h \
	table_stream.h


libgenutil_la_SOURCES = \
        Angle.C \
        angleconv.c \
	BatchQueue.C \
	BoxMuller.C \
        Cartesian.C \
        CommandParser.C \
        companion_mass.c \
        completion.C \
        compressed_io.C \
        coord.c \
        coord_parse.c \
	copy.C \
        dirglob.C \
	dirglobtree.C \
        dirutil.c \
        eccentric_anomaly.c \
        endian.c \
        errscale.c \
	expand.C \
	evaluate.C \
	ExponentialDistribution.C \
        fcomp.C \
	filesize.c \
	format_it.C \
        fsleep.c \
	Horizon.C \
        ieee.c \
        ierf.c \
        JenetAnderson98A5.C \
        JenetAnderson98.C \
        MJD.C \
	FITSUTC.C \
	NormalDistribution.C \
        Physical.C \
	Probability.C \
        Plugin.C \
        RealTimer.C \
        RegularExpression.C \
	replace_extension.C \
        sky_coord.C \
        sla_MJD.C \
        sla_sky_coord.C \
	smooth.f \
        stringload.C \
        stringprintf.C \
        stringtok.C \
        strutil.C \
	TemporaryFile.C \
	ThreadContext.C \
	time_string.C \
        Types.C \
        utc.c \
	VirtualMemory.C \
	table_stream.C

if HAVE_PGPLOT
include_HEADERS += JenetAnderson98Plot.h
libgenutil_la_SOURCES += JenetAnderson98Plot.C
endif

bin_PROGRAMS = getMJD getHorizon

getMJD_SOURCES = getMJD.C
getHorizon_SOURCES = getHorizon.C

TESTS = test_evaluate test_coord test_Cartesian test_MJD test_ja98 \
	test_endian test_environ test_sum test_NormalDistribution \
	test_TemporaryFile test_moment2 test_MJD_ostream test_sky_coord \
	test_exponential

#	test_BatchQueue

check_PROGRAMS = $(TESTS) test_CommandParser test_Angle test_expand

test_evaluate_SOURCES           = test_evaluate.C
test_Cartesian_SOURCES		= test_Cartesian.C
test_MJD_SOURCES		= test_MJD.C
test_Angle_SOURCES		= test_Angle.C
test_Angle_LDADD		= $(LDADD) @FLIBS@
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

# test_BatchQueue_SOURCES		= test_BatchQueue.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = @PGPLOT_CFLAGS@

LDADD = libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la

include $(top_srcdir)/config/Makefile.include

