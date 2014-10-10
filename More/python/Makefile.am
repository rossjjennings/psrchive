
nodist_python_PYTHON = psrchive.py

python_LTLIBRARIES = _psrchive.la

_psrchive_la_SOURCES = psrchive.i

nodist__psrchive_la_SOURCES = ./psrchive_wrap.cxx

_psrchive_la_LDFLAGS = -module -avoid-version -no-undefined

_psrchive_la_LIBADD = $(top_builddir)/More/libpsrmore.la \
                      $(top_builddir)/Base/libpsrbase.la \
                      $(top_builddir)/Util/libpsrutil.la @PYTHON_LDFLAGS@ 

SWIG_FLAGS = -Wall -python -I$(top_builddir)/local_include -I$(top_builddir)

./psrchive_wrap.cxx: $(srcdir)/psrchive.i
	$(SWIG) $(SWIG_FLAGS) $(DEFS) -outcurrentdir $(srcdir)/psrchive.i

CLEANFILES = psrchive.py psrchive.pyc ./psrchive_wrap.cxx

#############################################################################
#

INCLUDES = @SWIG_PYTHON_CPPFLAGS@ -I$(top_builddir)/local_include
AM_CPPFLAGS = @CFITSIO_CFLAGS@
AM_LDFLAGS = @CFITSIO_LDFLAGS@

