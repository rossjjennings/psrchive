
noinst_LTLIBRARIES = libepos.la

nobase_include_HEADERS = Pulsar/EPOSArchive.h

libepos_la_SOURCES = EPOSArchive.C eposio.c eposio.h

#############################################################################

include $(top_srcdir)/config/Makefile.include

#
# builddir != srcdir fix: unlike g77 version 3, gfortran does not look
# for header files (like epnhdr.inc) in the same path as the source code file.
#
AM_FFLAGS = -I$(srcdir)

