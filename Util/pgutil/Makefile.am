
noinst_LTLIBRARIES = libpgutil.la

include_HEADERS = ColourMap.h quickplot.h Plot3D.h BandpassPlotter.h

libpgutil_la_SOURCES = ColourMap.C quickimage.C Plot3D.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include @PGPLOT_CFLAGS@

include $(top_srcdir)/config/Makefile.include

