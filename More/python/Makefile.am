
lib_LTLIBRARIES   = _psrchive.la

_psrchive_la_SOURCES = psrchive_wrap.cxx qApp.C

_psrchive_la_LDFLAGS = -module -avoid-version -no-undefined

_psrchive_la_LIBADD = @PSRCHIVE_LIBS@

psrchive_wrap.cxx: psrchive.i ../Pulsar/Archive.h
	$(SWIG) -Wall -python -I.. psrchive.i

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = @PSRCHIVE_CFLAGS@ @SWIG_PYTHON_CPPFLAGS@

