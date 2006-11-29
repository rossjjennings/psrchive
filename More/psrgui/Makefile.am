
noinst_LTLIBRARIES = libpsrgui.la

include_HEADERS = PulsarGUI.h plotItem.h interfacePanel.h

libpsrgui_la_SOURCES = PulsarGUI.C plotItem.C interfacePanel.C

CLEANFILES = moc_PulsarGUI.C moc_plotItem.C moc_interfacePanel.C

nodist_libpsrgui_la_SOURCES = $(CLEANFILES)

bin_PROGRAMS = psrgui

psrgui_SOURCES = psrgui.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include @PGPLOT_CFLAGS@ @QT_CXXFLAGS@

LDADD = libpsrgui.la $(top_builddir)/More/libpsrmore.la \
        $(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la \
        @QT_LIBS@ @PGPLOT_LIBS@

include $(top_srcdir)/config/Makefile.Qt

