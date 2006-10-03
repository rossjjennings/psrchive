
bin_PROGRAMS = rhythm

rhythm_SOURCES = rhythm.C menubar.C toolbar.C command_line.C \
	Options.C toaPlot.C Options.h rhythm.h toaPlot.h pulsar.xpm

nodist_rhythm_SOURCES = moc_rhythm.C moc_Options.C moc_toaPlot.C

include $(top_srcdir)/config/Makefile.Qt

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include @QTDRIV_CFLAGS@

LDADD = ../libpsrchive.la $(top_builddir)/astro/libpsrastro.la \
	$(top_builddir)/utils/libpsrutils.la @QTDRIV_LIBS@

include $(top_srcdir)/config/Makefile.include

