LOCAL_ROOT = ../../..
include ${LOCAL_ROOT}/include/Makefile.standard

INCLUDES = superarchive.h archive.h sub_int.h profile.h archhdr.h headers.h \
        mini++.h timer++.h timer.h mini.h band.h archdefs.h \
	timer.inc band.inc mini.inc convert_endian.h misc.h \
	poln_profile.h Pulsar/TimerArchive.h Pulsar/TimerIntegration.h \
	Pulsar/BasebandArchive.h Pulsar/baseband_header.h

# ############################################################################
#
# The main timer archive library
#
# ############################################################################

LIBRARY := libarchive.a

LIB_SRCS = profile.C sub_int.C archive.C superarchive.C  \
	toa.C archive_toa.C sub_int_toa.C profile_toa.C \
	misc.C heapsort.C \
	archhdr.C convert_endian.c headers.C \
	timer++.C mini++.C poln_profile.C faraday_course.C \
	ffft.f fftfit.f brent.f fccf.f cprof.f shiftbyfft.f \
	width.f tsky1.f fftconv.f scan_fort_val.f convolve.f \
	galrot.C getvel.C


ifeq ($(has_PGPLOT),yes)
LIB_SRCS += profile_pgplot.C sub_int_pgplot.C archive_pgplot.C \
	profplot.C plot2profs.C scintplot.C 
endif

# ############################################################################
#
# The TimerArchive psrchive plugin
#
# ############################################################################

DSO_NAME := Pulsar/TimerArchive.so
SUBINCDIR := Pulsar

DSO_SRCS := BasebandArchive.C TimerArchive.C \
	TimerArchive_load.C TimerArchive_unload.C \
	TimerIntegration.C TimerIntegration_load.C TimerIntegration_unload.C \
	TimerProfile_load.C TimerProfile_unload.C \
	TimerAgent.C

LIB_SRCS += $(DSO_SRCS)

DSO_LDFLAGS := $(LDFLAGS) -lpsrchive_Extension -larchive $(LTIMES)

# TimerIntegration_cal_levels.C


# ############################################################################
#
# Executables
#
# ############################################################################

PROGRAMS = calinfo.C align.C amps.C chanprep.C crstd.C \
	mel.C newname.C oneprof.C plotsub.C tadd.C modhead.C \
	plot_gry_diff.C freduce.C cleanarch.C fixhdr.C crm.C  \
	crh.C psrhead.C fakedat.C plk.C stokes_plt.C \
	headsum.C treduce.C mel2.C invint.C \
	tcombine2.C ratio.C fixprf.C xformprof.C archive_test.C \
	h1_studies.C arch3d.C

TESTS = timer_size.c baseband_header_size.C

LDFLAGS += -larchive -ltempo++ -lpolyco -leph -ltimes \
	-lgenutil++ -lgenutil -lsla $(LFFT) $(LF77) -lm

oneprof.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
plotsub.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
plot_gry_diff.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
freduce.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
plk.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
stokes_plt.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
testbpass.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
ratio.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
treduce.LDFLAGS := $(LDFLAGS) $(LCPGPLOT)
h1_studies.LDFLAGS := $(LDFLAGS) $(LCPGPLOT) $(LSLA)
# ########################################################################
# treduce dialog stuff is not for linux
ifneq ($(LOGIN_ARCH),linux)
ifneq ($(LOGIN_ARCH),sungcc)
treduce.LDFLAGS := $(LDFLAGS) $(LDIALOG) -lgenutil $(LCPGPLOT) 
treduce.srcs = xtinit.C xtmac.C xtproc.C xtlplt.C xtinplot.C xtshdr.C    \
	       xtshst.C xtcopy.C xtswap.C xtunld.C xtgrey.C xtreduce.C   \
               newtreduce.f
endif
endif

include $(LOCAL_ROOT)/include/Makefile.extended
INCLUDES += pg_plot_gnu.h
