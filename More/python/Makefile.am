
lib_LTLIBRARIES   = _psrchive.la

_psrchive_la_SOURCES = psrchive_wrap.cxx

_psrchive_la_LDFLAGS = -module -avoid-version -no-undefined

_psrchive_la_LIBADD = @PSRCHIVE_LIBS@

psrchive_wrap.cxx: psrchive.i ../../local_include/Pulsar/Archive.h
	$(SWIG) -Wall -python -I../../local_include psrchive.i

#############################################################################
#

INCLUDES = @PSRCHIVE_CFLAGS@ @SWIG_PYTHON_CPPFLAGS@

