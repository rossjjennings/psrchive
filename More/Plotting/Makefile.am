
noinst_LTLIBRARIES = libPlotter.la

##############################################################################
#
# Pretty much everything is PGPLOT-specific code
#
if HAVE_PGPLOT

nobase_include_HEADERS = Pulsar/ProfilePlotter.h Pulsar/SinglePlotter.h

libPlotter_la_SOURCES = ProfilePlotter.C SinglePlotter.C

bin_PROGRAMS = psrplot

psrplot_SOURCES = psrplot.C

endif

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/utils/include -I$(top_builddir)/astro/include \
	-I$(top_srcdir)/psrchive @PGPLOT_CFLAGS@ @SLA_CFLAGS@

LDADD = libPlotter.la $(top_builddir)/psrchive/libpsrchive.la	\
	$(top_builddir)/astro/libpsrastro.la			\
	$(top_builddir)/utils/libpsrutils.la @PGPLOT_LIBS@
