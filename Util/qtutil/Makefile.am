
noinst_LTLIBRARIES = libqtutil.la

include_HEADERS = qt_value.h qt_double.h qtools.h

libqtutil_la_SOURCES = qt_value.C qt_double.C qtools.C
nodist_libqtutil_la_SOURCES = moc_qt_value.C moc_qt_double.C

check_PROGRAMS = qt_double_test

qt_double_test_SOURCES = qt_double_test.C

include $(top_srcdir)/config/Makefile.Qt

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = @QT_CXXFLAGS@

LDADD = @QT_LIBS@ 
