
noinst_LTLIBRARIES = libtempo2.la

include_HEADERS = T2Generator.h T2Predictor.h T2Parameters.h

libtempo2_la_SOURCES = T2Predictor.C T2Generator.C T2Parameters.C

check_PROGRAMS = test_T2Generator

test_T2Generator_SOURCES = test_T2Generator.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include @TEMPO2_CFLAGS@

LDADD = libtempo2.la \
	$(top_builddir)/Util/tempo/libtempo.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la @FLIBS@

libtempo2_la_LIBADD = @TEMPO2_LIBS@

include $(top_srcdir)/config/Makefile.include
