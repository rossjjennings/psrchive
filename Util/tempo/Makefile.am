include ${PSRHOME}/include/Makefile.${LOGIN_ARCH}
PWDIR = ${PSRHOME}/soft/models/eph

INCLUDES  = ephio.h

LIBRARY   = libeph.a
LIB_SRCS  = ephio_c.c psrmodel2.C ephio.f rnd8.f

PROGRAMS  = ephtest.c keys2c.c keys2fort.c eph2upd.f

LDFLAGS  += -leph $(F77LIBS) -lm

include ${PSRHOME}/include/Makefile.extended

CLEAN += $(BINL)ephtest $(BINL)keys2fort $(BINL)keys2c keyinfo.com ephio.h

#
# Assembly of include files
#

$(BINL)keys2c: keys.dat keys2c.c Makefile
	$(CC) -g -o $@ keys2c.c

$(BINL)keys2fort: keys2fort.c Makefile
	$(CC) -g -o $@ keys2fort.c

keyinfo.com: keys.dat keys2fort Makefile
	$(BINL)keys2fort

ephio.h: keys.dat keys2c
	$(BINL)keys2c

# force construction of that crazy ephio.h file
depend: ephio.h
includes: ephio.h

#
# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com
$(LOGIN_ARCH)/ephio_c.o: ephio.h
