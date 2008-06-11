
bin_PROGRAMS = psrchive_info psrchive_config

psrchive_info_SOURCES   = psrchive_info.C
psrchive_config_SOURCES = psrchive_config.C

bin_SCRIPTS = psrchive

dist_bin_SCRIPTS = psrchive_install

dist_data_DATA = psrchive_version

aclocaldir = $(datadir)/aclocal
dist_aclocal_DATA = psrchive.m4

htmldir = $(datadir)/html
dist_html_DATA = psrchive_layout.shtml

#############################################################################
#

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

libs include:
	@echo > /dev/null

