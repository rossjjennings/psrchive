
noinst_LTLIBRARIES = libqtutil.la

include_HEADERS = qt_value.h qt_double.h qtools.h qt_Angle.h qt_MJD.h

libqtutil_la_SOURCES = qt_value.C qt_double.C qtools.C qt_Angle.C qt_MJD.C

MOC_SOURCES = moc_qt_value.C moc_qt_double.C moc_qt_Angle.C moc_qt_MJD.C

nodist_libqtutil_la_SOURCES = $(MOC_SOURCES)

check_PROGRAMS = qt_double_test qt_Angle_test qt_MJD_test

qt_double_test_SOURCES = qt_double_test.C
qt_Angle_test_SOURCES = qt_Angle_test.C
qt_MJD_test_SOURCES = qt_MJD_test.C

include $(top_srcdir)/config/Makefile.Qt

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_srcdir)/utils/genutil -I$(top_srcdir)/utils/genutil++ \
	@QT_CXXFLAGS@

LDADD = libqtutil.la ../genutil++/libgenutilpp.la ../genutil/libgenutil.la \
	../units/libunits.la @QT_LIBS@ 


include $(top_srcdir)/config/Makefile.include

