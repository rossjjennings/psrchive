
bin_SCRIPTS = psrchive_cflags psrchive_ldflags

dist_bin_SCRIPTS = psrchive_install

dist_data_DATA = psrchive_version

libs: install-binSCRIPTS fix_prefix

install-exec-hook: fix_prefix

#
# The following lines ensure that the psrchive_cflags and psrchive_ldflags
# scripts are up-to-date with respect to where the software was installed.
#

FIX_PREFIX := ${prefix}/bin/psrchive_ldflags ${prefix}/bin/psrchive_cflags

fix_prefix:
	perl -p -i -e "s|PREFIX|${prefix}|" ${FIX_PREFIX}

