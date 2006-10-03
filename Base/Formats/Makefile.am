
noinst_LTLIBRARIES = libFormats.la
libFormats_la_SOURCES = Agent_static.C

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
	-I$(top_srcdir)/Core/Bases \
	-I$(top_srcdir)/Core/Extensions \
	-I$(top_srcdir)/Core/Formats/Timer \
	-I$(top_srcdir)/Core/Formats/EPN \
	-I$(top_srcdir)/Core/Formats/ASCII \
	-I$(top_srcdir)/Core/Formats/PSRFITS

include $(top_srcdir)/config/Makefile.local

