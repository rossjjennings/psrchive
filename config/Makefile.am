#############################################################################
#
# This Makefile smooths over various bugs and bumps and other features
# that break the other GNU autotools-generated Makefiles
#

all-local: 03-09-2007

libs:

#############################################################################
#
# 3 September 2007 - WvS
#
# After adding Management/psrchive_info.C make fails with
# ".deps/psrchive_info.Po: No such file or directory"
#
03-09-2007:
	@if test ! -f $(top_builddir)/Management/.deps/psrchive_info.Po; then \
	  cd $(top_builddir) && \
          /bin/sh ./config.status Management/Makefile depfiles; \
	fi

