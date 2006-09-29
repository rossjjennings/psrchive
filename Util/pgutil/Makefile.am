
noinst_LTLIBRARIES = libpgutil.la

include_HEADERS = ColourMap.h quickplot.h

libpgutil_la_SOURCES = ColourMap.C quickimage.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/utils/units \
	-I$(top_srcdir)/utils/genutil @PGPLOT_CFLAGS@

