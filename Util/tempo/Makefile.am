
noinst_LTLIBRARIES = libeph.la

include_HEADERS = psrephem.h ephio_func.h MatchingEphemeris.h

nodist_include_HEADERS = ephio.h

libpsrephem_la_SOURCES = ephio_c.c ephio.f \
	psrephem.C set_epoch.C tex.C derived.C MatchingEphemeris.C

nodist_libpsrephem_la_SOURCES = ephio_def.c

bin_PROGRAMS = keys2c keys2fort ephaux ephepo ephtex

ephaux_SOURCES			= ephaux.C
ephepo_SOURCES			= ephepo.C
ephtex_SOURCES			= ephtex.C


# ######################################################################
#
# PSRFITS specific code
#
if HAVE_CFITSIO

CFITSIO_SOURCES = fitsio.C

check_PROGRAMS = test_psrephem_fitsio.C

test_psrephem_fitsio_SOURCES	= test_psrephem_fitsio.C

endif


# ######################################################################
#
# Assembly of include files
#

keyinfo.com: keys.dat $(BINDIR)/keys2fort
	$(BINDIR)/keys2fort

ephio_def.c ephio.h: keys.dat $(BINDIR)/keys2c
	$(BINDIR)/keys2c

# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com

