
SUBDIRS = uvmlib

noinst_LTLIBRARIES = libuvm.la

libuvm_la_LIBADD = uvmlib/libuvm.la @FLIBS@
libuvm_la_SOURCES = uvmio.c uvmio.h

bin_PROGRAMS = uvmhdr

uvmhdr_SOURCES = uvmhdr.C

#############################################################################

LDADD = libuvm.la $(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include
