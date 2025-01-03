
nodist_python_PYTHON = psrchive.py

python_LTLIBRARIES = _psrchive.la

_psrchive_la_SOURCES = psrchive.i

nodist__psrchive_la_SOURCES = psrchive_wrap.cxx

_psrchive_la_LDFLAGS = -module -avoid-version -no-undefined

_psrchive_la_LIBADD = $(top_builddir)/More/libpsrmore.la \
                      $(top_builddir)/Base/libpsrbase.la \
                      $(top_builddir)/Util/libpsrutil.la \
                      $(top_builddir)/Util/epsic/src/util/libutil.la \
                      @PYTHON_LDFLAGS@ 

SWIG_FILE = $(srcdir)/psrchive.i
SWIG_FLAGS = -Wall -python -I$(top_builddir)/local_include -I$(top_builddir)
SWIG_DEPS = $(DEPDIR)/swig.deps

psrchive_wrap.cxx: $(SWIG_FILE) $(top_builddir)/config.h
	$(SWIG) $(SWIG_FLAGS) $(DEFS) -outcurrentdir $(SWIG_FILE)

#############################################################################
#
# regenerate psrchive_wrap.cxx and psrchive.py if C++ headers change
#

$(SWIG_DEPS): $(SWIG_FILE)
	$(SWIG) $(SWIG_FLAGS) $(DEFS) -MM -outcurrentdir $(SWIG_FILE) > $(SWIG_DEPS)

-include $(SWIG_DEPS)

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

CLEANFILES += psrchive.py psrchive.pyc psrchive_wrap.cxx $(SWIG_DEPS)

AM_CPPFLAGS += @SWIG_PYTHON_CPPFLAGS@ -I$(top_builddir)/local_include
AM_CPPFLAGS += @CFITSIO_CFLAGS@ @GSL_CFLAGS@
AM_LDFLAGS += @CFITSIO_LDFLAGS@

