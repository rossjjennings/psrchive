LOCAL_ROOT = ../../../
include $(LOCAL_ROOT)include/Makefile.${LOGIN_ARCH}

# PWDIR = ${PSRHOME}/soft/models/eph
# INCLUDES  = ephio.h

LIBRARY   = libeph.a
LIB_SRCS  = ephio_c.c psrmodel2.C ephio.f rnd8.f

PROGRAMS  = ephtest.c keys2c.c keys2fort.c eph2upd.f

LDFLAGS  += -leph $(F77LIBS) -lm

include ${LOCAL_ROOT}/include/Makefile.extended

CLEAN += $(BINDIR)/ephtest $(BINDIR)/keys2fort $(BINDIR)/keys2c keyinfo.com $(LOCAL_ROOT)/include/ephio.h

#
# Assembly of include files
#

$(BINDIR)/keys2c: keys2c.c Makefile
	$(CC) -g -o $@ keys2c.c

$(BINDIR)/keys2fort: keys2fort.c Makefile
	$(CC) -g -o $@ keys2fort.c

keyinfo.com: keys.dat $(BINDIR)/keys2fort Makefile
	$(BINDIR)/keys2fort

$(LOCAL_ROOT)include/ephio.h: keys.dat $(BINDIR)/keys2c
	$(BINDIR)/keys2c
	mv -f ephio.h $(LOCAL_ROOT)/include/ephio.h

# force construction of that crazy ephio.h file
depend: $(LOCAL_ROOT)/include/ephio.h
#includes: $(LOCAL_ROOT)/include/ephio.h

#
# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com
$(LOGIN_ARCH)/ephio_c.o: $(LOCAL_ROOT)include/ephio.h
