
SUBDIRS	= config packages Util Base More Management

include $(top_srcdir)/config/Makefile.root

ACLOCAL_AMFLAGS = -I config
AM_CPPFLAGS = @EPSIC_CFLAGS@

# make clean deletes the local_include directory (and any out-of-date headers)
clean-local: clean-local-include
.PHONY: clean-local-include
clean-local-include:
	-rm -rf local_include

