
noinst_LTLIBRARIES = libgenutilpp.la

include_HEADERS = Cartesian.h Angle.h MJD.h CommandParser.h		\
	PlotManager.h PlotVolume.h PlotEvent.h PlotState.h		\
	DataManager.h DataSet.h DataPoint.h DataVector.h Poincare.h	\
	datum.h xydatum.h smarthist.h datumcond.h kstest.h RealTimer.h	\
	Ready.h ReferenceFit.h sky_coord.h \
	PrimeFactoriser.h WindowArray.h StringTokenizer.h		\
	Sortable.h Printable.h Header.h thread_utils.h GetPK.h		\
	VectorInfo.h Plugin.h shell.h Cartesian.h Angle.h coord_parse.h \
	coord.h anglevel.h obsjoin.h casts.h

libgenutilpp_la_SOURCES = CommandParser.C PlotEvent.C util_plus.C	\
	PlotState.C datum.C smarthist.C datumcond.C kstest.C Ready.C	\
	PrimeFactoriser.C WindowArray.C StringTokenizer.C		\
	RealTimer.C Printable.C Header.C sky_coord.C sla_sky_coord.C	\
	thread_utils.C GetPK.C completion.C Cartesian.C Angle.C MJD.C	\
	sla_MJD.C PlotManager.C PlotVolume.C DataManager.C		\
	DataSet.C Poincare.C DataVector.C Plugin.C shell.C		\
	coord_parse.c coord.c sla_coord.c psr2ar.c dual_parse_it.h	\
	obsjoin.C

bin_PROGRAMS = getMJD tranScan bshead search_summary xserve_summary

# broken: loopy.C timeconvert.c

getMJD_SOURCES			= getMJD.C
tranScan_SOURCES		= tranScan.C
bshead_SOURCES			= bshead.C
search_summary_SOURCES		= search_summary.C
xserve_summary_SOURCES		= xserve_summary.C
#loopy_SOURCES			= loopy.C

TESTS = test_coord test_Cartesian test_MJD test_Angle test_ReferenceFit \
	test_Header test_VectorInfo

check_PROGRAMS = $(TESTS) test_CommandParser test_Sortable

test_Cartesian_SOURCES		= test_Cartesian.C
test_MJD_SOURCES		= test_MJD.C
test_Angle_SOURCES		= test_Angle.C
test_Angle_LDADD		= $(LDADD) @SLA_LIBS@
test_ReferenceFit_SOURCES	= test_ReferenceFit.C
test_Header_SOURCES		= test_Header.C
test_VectorInfo_SOURCES		= test_VectorInfo.C 
test_CommandParser_SOURCES      = test_CommandParser.C
test_Sortable_SOURCES           = test_Sortable.C

AM_CPPFLAGS = -D_REENTRANT=1

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/utils/units \
	-I$(top_srcdir)/utils/genutil

LDADD = libgenutilpp.la ../genutil/libgenutil.la ../units/libunits.la

