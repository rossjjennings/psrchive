
noinst_LTLIBRARIES = libpsrephem.la

include_HEADERS = psrephem.h ephio_func.h MatchingEphemeris.h

nodist_include_HEADERS = ephio.h

libpsrephem_la_SOURCES = keys.dat ephio_c.c ephio.f \
	psrephem.C set_epoch.C tex.C derived.C MatchingEphemeris.C

nodist_libpsrephem_la_SOURCES = ephio_def.c

bin_PROGRAMS = keys2c keys2fort ephaux ephepo ephtex

ephaux_SOURCES	= ephaux.C
ephaux_LDADD	= $(LDADD) @SLA_LIBS@

ephepo_SOURCES	= ephepo.C

ephtex_SOURCES	= ephtex.C
ephtex_LDADD	= $(LDADD) @SLA_LIBS@

# ######################################################################
#
# PSRFITS specific code
#
if HAVE_CFITSIO

libpsrephem_la_SOURCES += fitsio.C

check_PROGRAMS = test_psrephem_fitsio test_ephio

test_psrephem_fitsio_SOURCES = test_psrephem_fitsio.C
test_psrephem_fitsio_LDADD = $(LDADD) @CFITSIO_LIBS@

endif


# ######################################################################
#
# Assembly of include files
#

$(srcdir)/keyinfo.com: $(srcdir)/keys.dat ./keys2fort
	abs_builddir=`pwd` && cd $(srcdir) && $$abs_builddir/keys2fort

$(srcdir)/ephio.h: $(srcdir)/keys.dat ./keys2c
	abs_builddir=`pwd` && cd $(srcdir) && $$abs_builddir/keys2c

# Depend on constructed include files
ephio.lo: $(srcdir)/keyinfo.com
ephio_c.lo: $(srcdir)/ephio.h

BUILT_SOURCES = ephio.h
keys2fort_LDADD = 
keys2c_LDADD =  

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I. -I$(top_builddir)/utils/include \
	@CFITSIO_CFLAGS@ @MPI_CFLAGS@ @SLA_CFLAGS@

LDADD = libpsrephem.la $(top_builddir)/utils/libpsrutils.la @FLIBS@

