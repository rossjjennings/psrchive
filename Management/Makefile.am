
bin_PROGRAMS = psrchive_info psrchive_config

psrchive_info_SOURCES   = psrchive_info.C
psrchive_config_SOURCES = psrchive_config.C

bin_SCRIPTS = psrchive_cflags psrchive_ldflags

dist_bin_SCRIPTS = psrchive_install

dist_data_DATA = psrchive_version psrchive.cfg

libs: fix_prefix

install-exec-hook: fix_prefix

#
# The following lines ensure that the psrchive_cflags and psrchive_ldflags
# scripts are up-to-date with respect to where the software was installed.
#

FIX_PREFIX := ${prefix}/bin/psrchive_ldflags ${prefix}/bin/psrchive_cflags

fix_prefix: install-binSCRIPTS
	perl -p -i -e "s|PREFIX|${prefix}|" ${FIX_PREFIX}

#############################################################################
#
# Dependencies should never be changed
#

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la


include $(top_srcdir)/config/Makefile.include
