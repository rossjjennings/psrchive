LOCAL_ROOT = ../../..
include $(LOCAL_ROOT)/include/Makefile.standard

LIBRARY  = libeph.a
LIB_SRCS = psrephem.C ephio_c.c ephio.f rnd8.f

PROGRAMS = ephtest.c keys2c.c keys2fort.c eph2upd.f psrephem_test.C

LDFLAGS  += -leph -lpolyco -ltimes -lgenutil $(LGTKMM) $(F77LIBS) -lm

##########################################################################
# optional GTK-- Widget set
##########################################################################
ifeq ($(PSRGTKMM),yes)
NOMD_SRCS = gtk_test.C gtk_psrephem.C
PROGRAMS += gtk_test.C
LIB_SRCS += gtk_psrephem.C
INCFLAGS += $(GTKMM_CCFLAGS)
endif

include ${LOCAL_ROOT}/include/Makefile.extended

CLEAN += keyinfo.com $(LOCAL_ROOT)/include/ephio.h

#
# Assembly of include files
#

$(BINDIR)/keys2c: keys2c.c Makefile
	$(CC) -g -o $@ keys2c.c

$(BINDIR)/keys2fort: keys2fort.c Makefile
	$(CC) -g -o $@ keys2fort.c

keyinfo.com: keys.dat $(BINDIR)/keys2fort Makefile
	$(BINDIR)/keys2fort

$(LOCAL_ROOT)/include/ephio.h: keys.dat $(BINDIR)/keys2c
	$(BINDIR)/keys2c
	mv -f ephio.h $(LOCAL_ROOT)/include/ephio.h

# force construction of ephio.h file
depend: $(LOCAL_ROOT)/include/ephio.h

#
# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com
$(LOGIN_ARCH)/ephio_c.o: $(LOCAL_ROOT)/include/ephio.h

