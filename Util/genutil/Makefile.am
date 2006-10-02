
noinst_LTLIBRARIES = libgenutil.la

include_HEADERS = \
        Angle.h \
        Brent.h \
        Cartesian.h \
        CommandParser.h \
        compressed_io.h \
        coord.h \
        coord_parse.h \
        dirutil.h \
        environ.h \
        fcomp.h \
        fsleep.h \
	Horizon.h \
        ieee.h \
        ierf.h \
        JenetAnderson98A5.h \
        JenetAnderson98.h \
        JenetAnderson98Plot.h \
        machine_endian.h \
        MJD.h \
        orbital.h \
        Physical.h \
        Plugin.h \
        RealTimer.h \
        RegularExpression.h \
        sky_coord.h \
        slalib.h \
        string_utils.h \
        Telescope.h \
        templates.h \
        Types.h \
        typeutil.h \
        utc.h


libgenutil_la_SOURCES = \
        Angle.C \
        angleconv.c \
        Cartesian.C \
        CommandParser.C \
        companion_mass.c \
        completion.C \
        compressed_io.C \
        coord.c \
        coord_parse.c \
        dirglob.C \
        dirutil.c \
        eccentric_anomaly.c \
        endian.c \
        errscale.c \
        fcomp.C \
        fsleep.c \
	Horizon.C \
        ieee.c \
        ierf.c \
        JenetAnderson98A5.C \
        JenetAnderson98.C \
        JenetAnderson98Plot.C \
        MJD.C \
        Physical.C \
        Plugin.C \
        RealTimer.C \
        RegularExpression.C \
        sky_coord.C \
        sla_MJD.C \
        sla_sky_coord.C \
        sla_wrap.c \
        stringload.C \
        stringprintf.C \
        stringtok.C \
        string_utils.C \
        Telescope.C \
        Types.C \
        utc.c

if HAVE_SLA_FORTRAN

# build the C wrappers to the Fortran SLALIB bindings
include_HEADERS += slalib.h
libgenutil_la_SOURCES += sla_wrap.c

endif

if HAVE_PGPLOT

include_HEADERS += JenetAnderson98Plot.h
libgenutil_la_SOURCES += JenetAnderson98Plot.C

endif

bin_PROGRAMS = getMJD

getMJD_SOURCES			= getMJD.C

TESTS = test_coord test_Cartesian test_MJD test_ja98 test_endian

check_PROGRAMS = $(TESTS) test_CommandParser test_Angle

test_Cartesian_SOURCES		= test_Cartesian.C
test_MJD_SOURCES		= test_MJD.C
test_Angle_SOURCES		= test_Angle.C
test_Angle_LDADD		= $(LDADD) @SLA_LIBS@
test_CommandParser_SOURCES      = test_CommandParser.C
test_CommandParser_LDADD	= $(LDADD) @READLINE_LIBS@
test_ja98_SOURCES		= test_ja98.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir) \
	-I$(top_srcdir)/utils/units -I$(top_builddir)/utils/units \
	-I$(top_srcdir)/utils/genutil @PGPLOT_CFLAGS@

LDADD = libgenutil.la ../units/libunits.la

libgenutil_la_LIBADD = @SLA_LIBS@

