
ACLOCAL_AMFLAGS = -I config

SUBDIRS	= config Util Base More Management

EXTRA_DIST = config/install-intermediate

libs:
	$(MAKE) RECURSIVE_TARGETS="all-recursive libs-recursive" libs-recursive

libs-am:
	@echo
	@echo
	@echo "PSRCHIVE libraries have been installed in ${prefix}/lib"
	@echo
	@echo

include: install-data

all-am:
	@echo
	@echo
	@echo "PSRCHIVE successfully compiled.  Remember to \"make install\""
	@echo
	@echo

install-am:
	@echo
	@echo
	@echo "PSRCHIVE has been installed in ${prefix}"
	@echo
	@echo

where:
	@echo
	@echo "PSRCHIVE will be installed in ${prefix}"
	@echo

#
# Persist after an autoconf macro file is removed
#
$(top_srcdir)/config/%.m4:
	@echo Ignoring dependency on $@

