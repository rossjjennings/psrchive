
noinst_LTLIBRARIES = libpgutil.la

include_HEADERS = ColourMap.h PlotData.h SuperPlotData.h PlotParams.h \
	PlotSimpleData.h

libpgutil_la_SOURCES = pgplot_smarthist.C PlotData.C PlotSimpleData.C \
	SuperPlotData.C PlotParams.C ColourMap.C

bin_PROGRAMS = disthist

disthist_SOURCES = disthist.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_srcdir) -I../units -I../genutil -I../genutil++ \
	@PGPLOT_CFLAGS@

LDADD = libpgutil.la ../genutil++/libgenutilpp.la ../genutil/libgenutil.la \
	../units/libunits.la @PGPLOT_LIBS@ 









