
bin_PROGRAMS = psrchive_info psrchive_config

psrchive_info_SOURCES   = psrchive_info.C
psrchive_config_SOURCES = psrchive_config.C

bin_SCRIPTS = psrchive_cflags psrchive_ldflags psrchive_aclocal psrchive_ssi

dist_bin_SCRIPTS = psrchive_install

dist_data_DATA = psrchive_version

aclocaldir = $(datadir)/aclocal
dist_aclocal_DATA = psrchive.m4

ssidir = $(datadir)/ssi
dist_ssi_DATA = psrchive_layout.shtml

libs: fix_prefix

install-exec-hook: fix_prefix

#
# The following lines ensure that the psrchive_cflags,
# psrchive_ldflags, and psrchive_aclocal scripts are up-to-date with
# respect to where the software was installed.
#

FIX_PREFIX = \
	$(bindir)/psrchive_ldflags \
	$(bindir)/psrchive_cflags \
	$(bindir)/psrchive_aclocal \
	$(bindir)/psrchive_ssi

fix_prefix: install-binSCRIPTS
	perl -p -i -e "s|PREFIX|${prefix}|; s|INCLUDEDIR|${includedir}|" ${FIX_PREFIX}

#############################################################################
#

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

