LOCAL_ROOT = ../../..
include $(LOCAL_ROOT)/include/Makefile.standard

INCLUDES = psrephem.h ephio.h

LIBRARY  = libeph.a
LIB_SRCS = psrephem.C ephio_c.c ephio.f rnd8.f

PROGRAMS = ephtest.c keys2c.c keys2fort.c eph2upd.f 

TESTS = psrephem_test.C

LDFLAGS  += -leph -lpolyco -ltimes -lgenutil $(LGTKMM) $(F77LIBS) -lsla -lm

##########################################################################
# optional GTK-- Widget set
##########################################################################
ifeq ($(PSRGTKMM),yes)
INCLUDES += gtk_psrephem.h
NOMD_SRCS = gtk_test.C gtk_psrephem.C
PROGRAMS += gtk_test.C
LIB_SRCS += gtk_psrephem.C
INCFLAGS += $(GTKMM_CCFLAGS)
endif

##########################################################################
# optional Qt Widget set
##########################################################################
ifeq ($(PSRQT),yes)
INCLUDES += qt_psrephem.h
LIB_SRCS += qt_psrephem.C moc_qt_psrephem.C
INCFLAGS += $(IQT)
PROGRAMS += qt_psrephem_test.C
LDFLAGS  += $(LQT)
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

ephio.h: keys.dat $(BINDIR)/keys2c
	$(BINDIR)/keys2c

# force construction of ephio.h file
depend: ephio.h

# Dependency on constructed include files
$(LOGIN_ARCH)/ephio.o: keyinfo.com

