
noinst_LTLIBRARIES = libFormats.la
libFormats_la_SOURCES = Agent_static.C \
	Predictor_children.C Parameters_children.C

SUBDIRS = Timer EPN ASCII

libFormats_la_LIBADD = Timer/libtimer.la EPN/libepn.la ASCII/libascii.la

if HAVE_CFITSIO
SUBDIRS += PSRFITS 
libFormats_la_LIBADD += PSRFITS/libpsrfits.la
endif

SUBDIRS += .

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include \
	-I$(top_srcdir)/Base/Formats/Timer \
	-I$(top_srcdir)/Base/Formats/EPN \
	-I$(top_srcdir)/Base/Formats/ASCII \
	-I$(top_srcdir)/Base/Formats/PSRFITS \
	@CFITSIO_CFLAGS@ @TEMPO2_CFLAGS@

include $(top_srcdir)/config/Makefile.local

