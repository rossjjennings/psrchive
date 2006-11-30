
ACLOCAL_AMFLAGS = -I config

SUBDIRS	= Util Base More

bin_SCRIPTS = psrchive_cflags psrchive_ldflags

EXTRA_DIST = config/install-intermediate

libs: install-binSCRIPTS
	$(MAKE) RECURSIVE_TARGETS="all-recursive libs-recursive" libs-recursive

libs-am:
	@echo
	@echo PSRCHIVE libraries have been installed in ${prefix}/lib
	@echo

include: install-data

FIX_PREFIX := ${prefix}/bin/psrchive_ldflags ${prefix}/bin/psrchive_cflags

install-exec-hook:
	perl -p -i -e "s|PREFIX|${prefix}|" ${FIX_PREFIX}

