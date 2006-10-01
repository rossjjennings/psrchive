
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

INCLUDES = -I$(top_builddir)/include \
        -I$(top_srcdir)/More/Plotter @PGPLOT_CFLAGS@ @QT_CXXFLAGS@

LDADD = libpsrgui.la $(top_builddir)/More/libpsrmore.la \
        ../libpsrchive.la $(top_builddir)/astro/libpsrastro.la \
        $(top_builddir)/utils/libpsrutils.la \
        @QT_LIBS@ @PGPLOT_LIBS@

