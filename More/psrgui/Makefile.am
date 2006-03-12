
noinst_LTLIBRARIES = libpsrgui.la

include_HEADERS = PulsarGUI.h plotItem.h interfacePanel.h

libpsrgui_la_SOURCES = PulsarGUI.C plotItem.C interfacePanel.C

nodist_libpsrgui_la_SOURCES = moc_PulsarGUI.C moc_plotItem.C \
                              moc_interfacePanel.C

bin_PROGRAMS = psrgui

psrgui_SOURCES = psrgui.C

include $(top_srcdir)/config/Makefile.Qt

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/utils/include -I$(top_builddir)/astro/include \
        -I$(top_srcdir)/psrchive -I$(top_srcdir)/psrchive/Extension \
        -I$(top_srcdir)/psrchive/Plotter @PGPLOT_CFLAGS@ @QT_CXXFLAGS@

LDADD = libpsrgui.la $(top_builddir)/psrchive/Plotter/libPlot.la \
        ../libpsrchive.la $(top_builddir)/astro/libpsrastro.la \
        $(top_builddir)/utils/libpsrutils.la \
        @QT_LIBS@ @PGPLOT_LIBS@

