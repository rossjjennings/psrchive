
SUBDIRS	= config packages Util Base More Management

include $(top_srcdir)/config/Makefile.root

ACLOCAL_AMFLAGS = -I config
AM_CPPFLAGS = @EPSIC_CFLAGS@

# make clean deletes the local_include directory (and any out-of-date headers)
clean-local: clean-local-include
.PHONY: clean-local-include
clean-local-include:
	-rm -rf local_include

dist-hook:
	@echo "Checking Autoconf version..."
	@if [ `echo "@AUTOCONF_VERSION@ < 2.71" | bc -l` -eq 1 ]; then \
	    echo; \
	    echo; \
	    echo "Error: Autoconf @AUTOCONF_VERSION@ is out of date. Please upgrade to 2.71 or greater before running 'make dist'." >&2; \
	    echo; \
	    echo; \
	    exit 1; \
	fi

