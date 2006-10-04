
noinst_LTLIBRARIES = libtempo.la

include_HEADERS = psrephem.h ephio_func.h psrephem_orbital.h \
	polyco.h Phase.h toa.h residual.h resio.h tempo++.h Predict.h

nodist_include_HEADERS = ephio.h

libtempo_la_SOURCES = keys.dat ephio_c.c ephio.f \
	psrephem.C set_epoch.C tex.C derived.C psrephem_orbital.C \
	getlun.f length.f posparse.f strmatch.f upcase.f zeropad.f \
	polyco.C Phase.C toa.C residual.C resio.f \
	tempo++.C Predict.C fit.C

nodist_libtempo_la_SOURCES = ephio_def.c

bin_PROGRAMS = keys2c keys2fort ephepo ephtex
check_PROGRAMS = test_ephio test_polyco_io test_Phase

ephepo_SOURCES = ephepo.C
ephtex_SOURCES = ephtex.C
test_polyco_io_SOURCES = test_polyco_io.C
test_Phase_SOURCES = test_Phase.C

ephtex_LDADD	= $(LDADD) @SLA_LIBS@

# ######################################################################
#
# PSRFITS specific code
#
if HAVE_CFITSIO

libtempo_la_SOURCES += fitsio_psrephem.C fitsio_polyco.C

check_PROGRAMS += test_psrephem_fitsio test_polyco_fitsio

test_psrephem_fitsio_SOURCES = test_psrephem_fitsio.C
test_polyco_fitsio_SOURCES = test_polyco_fitsio.C
test_psrephem_fitsio_LDADD = $(LDADD) @CFITSIO_LIBS@
test_polyco_fitsio_LDADD = $(LDADD) @CFITSIO_LIBS@

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

INCLUDES = -I$(top_builddir)/include @CFITSIO_CFLAGS@ @SLA_CFLAGS@

LDADD = libtempo.la $(top_builddir)/utils/genutil/libgenutil.la \
	$(top_builddir)/utils/units/libunits.la \
	$(top_builddir)/utils/fitsutil/libfitsutil.la @FLIBS@

include $(top_srcdir)/config/Makefile.include

