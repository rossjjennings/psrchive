
ACLOCAL_AMFLAGS = -I config

SUBDIRS	= Util Base More

bin_SCRIPTS = psrchive_cflags psrchive_ldflags

EXTRA_DIST = config/install-intermediate

libs: install-binSCRIPTS
	$(MAKE) RECURSIVE_TARGETS="all-recursive libs-recursive" libs-recursive

libs-am: fix_prefix
	@echo
	@echo PSRCHIVE libraries have been installed in ${prefix}/lib
	@echo

include: install-data

install-exec-hook: fix_prefix reminder

#
# The following lines ensure that the psrchive_cflags and psrchive_ldflags
# scripts are up-to-date with respect to where the software was installed.
#

FIX_PREFIX := ${prefix}/bin/psrchive_ldflags ${prefix}/bin/psrchive_cflags

fix_prefix:
	perl -p -i -e "s|PREFIX|${prefix}|" ${FIX_PREFIX}

#
# The following lines print a reminder to the user after the installation
# is completed.
#

reminder:
	@echo
	@echo "PSRCHIVE has been installed in ${prefix}"
	@echo
	@echo "Remember to set the PSRCHIVE environment variable; e.g."
	@echo
	@echo "in your .cshrc file:"
	@echo "  setenv PSRCHIVE ${prefix}"
	@echo
	@echo "or, in your .bashrc file:"
	@echo "  export PSRCHIVE=${prefix}"
	@echo
