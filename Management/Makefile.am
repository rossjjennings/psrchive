
TESTS = psrchive_config

bin_PROGRAMS = psrchive_info 

psrchive_info_SOURCES   = psrchive_info.C

if !MAKE_LEAN
bin_PROGRAMS += psrchive_config
psrchive_config_SOURCES = psrchive_config.C Config_ensure_linkage.C
endif

bin_SCRIPTS = psrchive

noinst_SCRIPTS = release.csh

dist_bin_SCRIPTS = psrchive_install

INSTALL_SCRIPT = $(srcdir)/install_with_githash.sh
EXTRA_DIST = install_with_githash.sh

dist_data_DATA = psrchive_version

aclocaldir = $(datadir)/aclocal
dist_aclocal_DATA = psrchive.m4

htmldir = $(datadir)/html
dist_html_DATA = psrchive_layout.shtml psrchive_top.shtml psrchive_side.shtml

#############################################################################
#

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la \
	$(top_builddir)/Util/epsic/src/libepsic.la

LDPLOT = $(top_builddir)/More/Plotting/libpsrplot.la $(LDADD) @PGPLOT_LIBS@

if HAVE_PGPLOT
psrchive_config_LDADD = $(LDPLOT)
else
psrchive_config_LDADD = $(LDADD)
endif


include $(top_srcdir)/config/Makefile.include

dist-hook:
	-chmod +x $(noinst_SCRIPTS)

libs include:
	@echo > /dev/null

