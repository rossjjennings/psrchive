LOCAL_ROOT = ../../..
include $(LOCAL_ROOT)/include/Makefile.standard

INCLUDES = psrephem.h ephio.h ephio_func.h
NONMODULE = ephio.h

LIBRARY  = libeph.a
LIB_SRCS = psrephem.C ephio_c.c ephio.f rnd8.f set_epoch.C tex.C

PROGRAMS = keys2c.c keys2fort.c eph2upd.f ephaux.C ephepo.C ephtex.C

TESTS = ephtest.c psrephem_test.C
psrephem_test.args := 0437-4715

ephtest.LDFLAGS := $(LDFLAGS) -leph -lgenutil $(F77LIBS) -lm

LDFLAGS += -leph -lpolyco $(LTIMES) -lgenutil $(F77LIBS) -lm

ephtex.LDFLAGS := $(LDFLAGS) -lpsrinfo -leph

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

ephio.h: keys.dat $(BINDIR)/keys2c
	$(BINDIR)/keys2c

# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com

