
bin_PROGRAMS = rhythm

rhythm_SOURCES = rhythm.C menubar.C toolbar.C command_line.C \
	Options.C toaPlot.C Options.h rhythm.h toaPlot.h pulsar.xpm \
	psrParams.C psrParams.h psrParameter.C psrParameter.h \
	find_standard.C qt_psrParameter.C qt_psrephem.C \
	qt_editParams.C qt_toglParams.C qt_fileParams.C 

nodist_rhythm_SOURCES = moc_rhythm.C moc_Options.C moc_toaPlot.C \
	moc_qt_psrephem.C moc_qt_editParams.C \
	moc_qt_toglParams.C moc_qt_fileParams.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include @QTDRIV_CFLAGS@

LDADD = $(top_builddir)/More/libpsrmore.la \
        $(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la @QTDRIV_LIBS@

include $(top_srcdir)/config/Makefile.Qt

