#############################################################################
# include "Master" makefile which defines important parameters
LOCAL_ROOT = ../../../
include ${LOCAL_ROOT}/include/Makefile.standard

LIBRARY    = libarchive.a
LIB_C_SRCS = pg_plot_gnu.C profile.C sub_int.C archive.C                \
             superarchive.C toa.C xtinit.C xtmac.C                      \
	     xtproc.C xtlplt.C xtinplot.C xtshdr.C xtshst.C xtcopy.C    \
             xtswap.C xtunld.C xtgrey.C xtreduce.C profplot.C           \
	     plot2profs.C scintplot.C misc.C heapsort.C                 \
                          get_epoch_and_name.C microhdr.C               \
	     convert_endian.C headers.C cal_fns.C cpg_fns.C iono.C     

LIB_F_SRCS = ffft.f fftfit.f brent.f fccf.f cprof.f shiftbyfft.f \
             convolve.f width.f tsky1.f newtreduce.f \
             fftconv.f scan_fort_val.f iono_rm.f ionrm.f 

LIB_SRCS   = ${LIB_C_SRCS} ${LIB_F_SRCS}

LDFLAGS   += -larchive -lpolyco -ldialog -lgenutil -ltimes \
             -lpsrinfo -leph -lsla -lpsrfft -lfftw -lm $(LIBNSL) \
	     -lcpgplot $(LPGPLOT) 

PROGRAMS =  = align.C amps.C chanprep.C  \
	      mel.C newname.C oneprof.C plotsub.C    \
	      tadd.C calfbar.C modhead.C   \
	      plot_gry_diff.C pepoch.C stokes_plt.C freduce.C \
	      cleanarch.C fixhdr.C crm.C  \
	      crh.C psrhead.C autotadd.C \
	      fakedat.C quik_cal.C plk.C \
	      cal_arch.C fitmix.C calplt.C vargain.C \
	      headsum.C iono_model.C treduce.C plk.C \
              tcombine2.C ratio.C

NOT_WORKING     = bandpass.C detail.C xtoa.C bin_ph.C corflux.C flux.C

include $(LOCAL_ROOT)/include/Makefile.extended




