#############################################################################
# include "Master" makefile which defines important parameters
LOCAL_ROOT = ../../../
include ${LOCAL_ROOT}/include/Makefile.standard

LIBRARY    = libarchive.a
LIB_C_SRCS = pg_plot_gnu.C profile.C sub_int.C archive.C superarchive.C  \
             toa.C profplot.C plot2profs.C scintplot.C misc.C heapsort.C \
             get_epoch_and_name.C archhdr.C convert_endian.C headers.C 

LIB_F_SRCS = ffft.f fftfit.f brent.f fccf.f cprof.f shiftbyfft.f \
             width.f tsky1.f fftconv.f scan_fort_val.f convolve.f

LIB_SRCS   = ${LIB_C_SRCS} ${LIB_F_SRCS}

LDFLAGS   += -larchive -lpolyco -ldialog -ltimes \
             -lpsrinfo -leph -lsla -lpsrfft -lgenutil -lfftw -lm $(LIBNSL) \
	     -lcpgplot $(LPGPLOT) 

treduce.OBJS = xtinit.o xtmac.o xtproc.o xtlplt.o xtinplot.o xtshdr.o    \
	       xtshst.o xtcopy.o xtswap.o xtunld.o xtgrey.o xtreduce.o   \
               newtreduce.o

PROGRAMS =    calinfo.C align.C amps.C chanprep.C  \
	      mel.C newname.C oneprof.C plotsub.C    \
	      tadd.C calfbar.C modhead.C   \
	      plot_gry_diff.C pepoch.C freduce.C \
	      cleanarch.C fixhdr.C crm.C  \
	      crh.C psrhead.C autotadd.C \
	      fakedat.C plk.C \
	      headsum.C treduce.C \
              tcombine2.C ratio.C fixprf.C 

NOT_WORKING     = bandpass.C detail.C xtoa.C bin_ph.C corflux.C flux.C vargain.C \
                  
include $(LOCAL_ROOT)/include/Makefile.extended

