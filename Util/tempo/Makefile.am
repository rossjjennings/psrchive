LOCAL_ROOT = ../../..
include $(LOCAL_ROOT)/include/Makefile.standard

INCLUDES = psrephem.h ephio.h
NONMODULE = ephio.h

LIBRARY  = libeph.a
LIB_SRCS = psrephem.C ephio_c.c ephio.f rnd8.f

PROGRAMS = ephtest.c keys2c.c keys2fort.c eph2upd.f 

TESTS = psrephem_test.C
psrephem_test.args := 0437-4715

ephtest.LDFLAGS := $(LDFLAGS) -leph -lgenutil $(F77LIBS) -lm

LDFLAGS += -leph -lpolyco -ltimes -lgenutil $(F77LIBS) -lm

##########################################################################
# optional GTK-- Widget set
##########################################################################
ifeq (${PSRGTKMM},yes)

INCLUDES += gtk_psrephem.h
NOMD_SRCS = gtk_test.C gtk_psrephem.C

LIB_SRCS += gtk_psrephem.C
INCFLAGS += $(GTKMM_CCFLAGS)

TESTS += gtk_test.C
gtk_test.LDFLAGS := $(LDFLAGS) $(LGTKMM)

endif


##########################################################################
# optional Qt Widget set
##########################################################################
ifeq (${PSRQT},yes)

INCLUDES += qt_psrephem.h
LIB_SRCS += qt_psrephem.C qt_ephembox.C qt_ephemVal.C \
	moc_qt_psrephem.C moc_qt_ephembox.C moc_qt_ephemVal.C

INCFLAGS += $(IQT)

TESTS += qt_psrephem_test.C
qt_psrephem_test.LDFLAGS := $(LDFLAGS) $(LQT) -lqtutil

endif

include $(LOCAL_ROOT)/include/Makefile.extended

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

