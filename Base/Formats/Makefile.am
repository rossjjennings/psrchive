
noinst_LTLIBRARIES = libFormats.la
libFormats_la_SOURCES = Agent_static.C \
	Predictor_children.C Parameters_children.C Generator_default.C

SUBDIRS = Timer EPN ASCII BPP WAPP

libFormats_la_LIBADD = Timer/libtimer.la EPN/libepn.la ASCII/libascii.la \
		       BPP/libbpp.la WAPP/libwapp.la

if HAVE_CFITSIO
SUBDIRS += PSRFITS ASP
libFormats_la_LIBADD += PSRFITS/libpsrfits.la ASP/libasp.la
endif

if HAVE_PUMA
SUBDIRS += PuMa
libFormats_la_LIBADD += PuMa/libpuma.la
endif

SUBDIRS += .

check_PROGRAMS = test_Parameters_factory test_Predictor_factory

test_Parameters_factory_SOURCES = test_Parameters_factory.C
test_Predictor_factory_SOURCES = test_Predictor_factory.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/local_include \
	-I$(top_srcdir)/Base/Formats/Timer \
	-I$(top_srcdir)/Base/Formats/EPN \
	-I$(top_srcdir)/Base/Formats/ASCII \
	-I$(top_srcdir)/Base/Formats/BPP \
	-I$(top_srcdir)/Base/Formats/WAPP \
	-I$(top_srcdir)/Base/Formats/PSRFITS \
	-I$(top_srcdir)/Base/Formats/ASP \
	@CFITSIO_CFLAGS@ @PUMA_CFLAGS@ @TEMPO2_CFLAGS@

include $(top_srcdir)/config/Makefile.local

LDADD = libFormats.la $(top_builddir)/Base/Classes/libClasses.la $(top_builddir)/Util/libpsrutil.la

