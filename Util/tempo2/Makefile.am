
noinst_LTLIBRARIES = libtempo2.la

include_HEADERS = T2Predictor.h

libtempo2_la_SOURCES = T2Predictor.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include

LDADD = libtempo2.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la @FLIBS@

include $(top_srcdir)/config/Makefile.include
