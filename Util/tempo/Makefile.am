LOCAL_ROOT = ../../..
include $(LOCAL_ROOT)/include/Makefile.standard

NONMODULE = ephio.h ephio_def.c

INCLUDES = psrephem.h ephio.h ephio_func.h

LIBRARY  = libeph.a
LIB_SRCS = ephio_c.c ephio_def.c ephio.f rnd8.f psrephem.C set_epoch.C \
	tex.C derived.C

PROGRAMS = keys2c.c keys2fort.c eph2upd.f ephaux.C ephepo.C ephtex.C

TESTS = testrnd8.f psrephem_test.C ephtest.C

psrephem_test.args := 0437-4715

LDFLAGS += -leph -lgenutil $(LSLA)

ephtex.LDFLAGS  := $(LDFLAGS) -lpsrinfo -leph $(LF77)
ephepo.LDFLAGS  := $(LDFLAGS) $(LTIMES)

# ######################################################################
# PSRFITS specific code
#
ifeq (${PSRFITS},yes)

LIB_SRCS += fitsio.C

TESTS += test_psrephem_fitsio.C
test_psrephem_fitsio.LDFLAGS := $(LDFLAGS) $(LTIMES) $(LCFITSIO)

endif
# ######################################################################

include $(LOCAL_ROOT)/include/Makefile.extended

ephtest : t.eph

CLEAN += keyinfo.com $(LOCAL_ROOT)/include/ephio.h

#
# Assembly of include files
#

$(BINDIR)/keys2c: keys2c.c
	$(CC) -g -o $@ keys2c.c

$(BINDIR)/keys2fort: keys2fort.c
	$(CC) -g -o $@ keys2fort.c

keyinfo.com: keys.dat $(BINDIR)/keys2fort
	$(BINDIR)/keys2fort

ephio_def.c ephio.h: keys.dat $(BINDIR)/keys2c
	$(BINDIR)/keys2c

# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com

