
SUBDIRS	= config packages Util Base More Management

$(RECURSIVE_TARGETS): check-submodules
.PHONY: force-reconfig check-submodules

SUBMODULE_MAKEFILES = Util/epsic/Makefile Util/meal/Makefile

check-submodules:
	@need_reconfig=no; \
	for f in $(SUBMODULE_MAKEFILES); do \
	  if [ ! -f "$(top_builddir)/$$f" ]; then need_reconfig=yes; break; fi; \
	done; \
	if [ "$$need_reconfig" = "yes" ]; then \
	  $(MAKE) $(AM_MAKEFLAGS) force-reconfig; \
	fi

force-reconfig:
	@echo "Missing submodule Makefiles detected. Reconfiguring project..."
	$(top_srcdir)/configure $(shell ./config.status --config)

include $(top_srcdir)/config/Makefile.root

ACLOCAL_AMFLAGS = -I config
AM_CPPFLAGS = @EPSIC_CFLAGS@

# make clean deletes the local_include directory (and any out-of-date headers)
clean-local: clean-local-include
.PHONY: clean-local-include
clean-local-include:
	-rm -rf local_include

if DISABLE_DIST
dist:
	@echo "'make dist' is disabled for Automake versions < 1.17"
endif

# For building the builder Docker image used in GitLab
COMMONS_NAME := psrchive
include $(top_srcdir)/.build-commons/docker/Makefile

# For running clang-tidy
include $(top_srcdir)/.build-commons/clang-tidy/Makefile

