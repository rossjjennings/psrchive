LOCAL_ROOT := ../../..
include $(LOCAL_ROOT)/include/Makefile.standard

NONMODULE := ephio.h ephio_def.c

INCLUDES := psrephem.h ephio.h ephio_func.h

LIBRARY  := libeph.a
LIB_SRCS := ephio_c.c ephio_def.c ephio.f rnd8.f \
	psrephem.C set_epoch.C tex.C derived.C

PROGRAMS := keys2c.c keys2fort.c eph2upd.f ephaux.C ephepo.C ephtex.C
LDFLAGS += $(F77LIBS) -lm

# test programs are intended to be run-alone (ie. no user input required)
# TESTS := testrnd8.f

eph2upd.LDFLAGS := $(LDFLAGS) -leph -lgenutil
ephaux.LDFLAGS  := $(LDFLAGS) -leph -lgenutil $(LSLA)
ephtex.LDFLAGS  := $(LDFLAGS) -leph -lpsrinfo -leph -lgenutil $(LSLA)
ephepo.LDFLAGS  := $(LDFLAGS) -leph -lgenutil $(LTIMES)
testrnd8.LDFLAGS:= $(LDFLAGS) -leph

# ######################################################################
# PSRFITS specific code
#
ifeq (${PSRFITS},yes)

LIB_SRCS += fitsio.C
TESTS += test_psrephem_fitsio.C
test_psrephem_fitsio.LDFLAGS := $(LDFLAGS) -leph $(LTIMES) $(LCFITSIO)

endif
# ######################################################################

include $(LOCAL_ROOT)/include/Makefile.extended

CLEAN += keyinfo.com $(LOCAL_ROOT)/include/ephio.h

#
# Assembly of include files
#

keyinfo.com: keys.dat $(BINDIR)/keys2fort
	$(BINDIR)/keys2fort

ephio_def.c ephio.h: keys.dat $(BINDIR)/keys2c
	$(BINDIR)/keys2c

# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com
