
noinst_LTLIBRARIES = libgenutilpp.la

include_HEADERS = ReferenceFit.h CommandParser.h			 \
	PlotManager.h PlotVolume.h PlotEvent.h PlotState.h		 \
	DataManager.h DataSet.h DataPoint.h DataVector.h Poincare.h	 \
	datum.h xydatum.h smarthist.h datumcond.h kstest.h RealTimer.h	 \
	Ready.h	 \
	WindowArray.h StringTokenizer.h					 \
	Sortable.h Printable.h Header.h thread_utils.h GetPK.h		 \
	VectorInfo.h Plugin.h shell.h Cartesian.h Angle.h coord_parse.h

libgenutilpp_la_SOURCES = CommandParser.C PlotEvent.C util_plus.C	\
	PlotState.C datum.C smarthist.C datumcond.C kstest.C Ready.C	\
	PrimeFactoriser.C WindowArray.C StringTokenizer.C		\
	RealTimer.C Printable.C Header.C				\
	thread_utils.C GetPK.C completion.C Cartesian.C Angle.C		\
	PlotManager.C PlotVolume.C DataManager.C			\
	DataSet.C Poincare.C DataVector.C Plugin.C shell.C		\
	coord_parse.c

# needs a new home: obsjoin.[hC]

bin_PROGRAMS = test_CommandParser test_Sortable tranScan bshead \
	search_summary xserve_summary

# broken: loopy.C

test_CommandParser_SOURCES	= test_CommandParser.C
test_Sortable_SOURCES		= test_Sortable.C
tranScan_SOURCES		= tranScan.C
bshead_SOURCES			= bshead.C
search_summary_SOURCES		= search_summary.C
xserve_summary_SOURCES		= xserve_summary.C
#loopy_SOURCES			= loopy.C

TESTS = test_ReferenceFit test_Header test_VectorInfo

check_PROGRAMS = $(TESTS)

test_ReferenceFit_SOURCES	= test_ReferenceFit.C
test_Header_SOURCES		= test_Header.C
test_VectorInfo_SOURCES		= test_VectorInfo.C 

AM_CPPFLAGS = -D_REENTRANT=1

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_srcdir) -I../units -I../genutil

LDADD = libgenutilpp.la ../genutil/libgenutil.la ../units/libunits.la

