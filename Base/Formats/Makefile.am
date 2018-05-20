
noinst_LTLIBRARIES = libFormats.la

libFormats_la_SOURCES = Agent_static.C

SUBDIRS = Timer EPN ASCII BPP WAPP PRESTO UVM

libFormats_la_LIBADD = Timer/libtimer.la EPN/libepn.la ASCII/libascii.la \
       BPP/libbpp.la WAPP/libwapp.la PRESTO/libpresto.la UVM/libuvm.la

if HAVE_CFITSIO
SUBDIRS += PSRFITS ASP
libFormats_la_LIBADD += PSRFITS/libpsrfits.la ASP/libasp.la
endif

if HAVE_PUMA
SUBDIRS += PuMa
libFormats_la_LIBADD += PuMa/libpuma.la
endif

SUBDIRS += .

#############################################################################
#

AM_CPPFLAGS = -I$(top_builddir)/local_include \
	-I$(top_srcdir)/Base/Formats/Timer \
	-I$(top_srcdir)/Base/Formats/EPN \
	-I$(top_srcdir)/Base/Formats/ASCII \
	-I$(top_srcdir)/Base/Formats/BPP \
	-I$(top_srcdir)/Base/Formats/WAPP \
	-I$(top_srcdir)/Base/Formats/PRESTO \
	-I$(top_srcdir)/Base/Formats/PSRFITS \
	-I$(top_srcdir)/Base/Formats/ASP

include $(top_srcdir)/config/Makefile.local

AM_CPPFLAGS += @CFITSIO_CFLAGS@ @PUMA_CFLAGS@ @FFT_CFLAGS@

