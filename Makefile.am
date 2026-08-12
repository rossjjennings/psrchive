
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

########################################
#
# clang-tidy config
#
########################################

.PHONY: tidy tidy-branch tidy-all

# Default to local 'main', but allow CI to override it
BASE_BRANCH ?= main

# Default to the number of CPU cores available on the machine
TIDY_JOBS ?= $(shell nproc)

# Generates the compilation database in the current build directory
compile_commands.json:
	bear $(MAKE) $(AM_MAKEFLAGS)

# Runs clang-tidy ONLY on uncommitted changes
tidy: compile_commands.json
	@echo "Running clang-tidy on uncommitted changes..."
	cd $(top_srcdir) && git diff -U0 HEAD | clang-tidy-diff -j $(TIDY_JOBS) -p1 -path $(abs_top_builddir)

# Runs clang-tidy on changes between the current branch and main
tidy-branch: compile_commands.json
	@echo "Running clang-tidy on branch changes..."
	cd $(top_srcdir) && git diff -U0 $(BASE_BRANCH) ..HEAD | clang-tidy-diff -j $(TIDY_JOBS) -p1 -path $(abs_top_builddir)

# Runs clang-tidy on ALL source files in the tree
tidy-all: compile_commands.json
	@echo "Running clang-tidy on ALL source files..."
	cd $(top_srcdir) && git ls-files '*.c' '*.C' '*.cpp' '*.h' | xargs -n 5 -P $(TIDY_JOBS) clang-tidy -p $(abs_top_builddir)

