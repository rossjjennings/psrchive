#############################################################################
# include "Master" makefile which defines important parameters
LOCAL_ROOT = ../../../
include ${LOCAL_ROOT}/include/Makefile.standard

LIBRARY    = libarchive.a
LIB_C_SRCS = pg_plot_gnu.C profile.C sub_int.C archive.C                \
             superarchive.C toa.C xtinit.C xtmac.C                      \
	     xtproc.C xtlplt.C xtinplot.C xtshdr.C xtshst.C xtcopy.C    \
             xtswap.C xtunld.C xtgrey.C xtreduce.C profplot.C           \
	     plot2profs.C scintplot.C misc.C heapsort.C calarsub.C      \
             plotcalsub.C get_epoch_and_name.C mkarchive.C microhdr.C   \
	     convert_endian.C main_hdr.C cal_fns.C cpg_fns.C
LIB_F_SRCS = ffft.f fftfit.f brent.f fccf.f cprof.f shiftbyfft.f \
             convolve.f width.f newtreduce.f tsky1.f \
             fftconv.f scan_fort_val.f iono_rm.f ionrm.f 

LIB_SRCS   = ${LIB_C_SRCS} ${LIB_F_SRCS}

LDFLAGS   += -larchive -ldialog -lpolyco -lgenutil -ltimes \
             -lpsrclock -lpsrinfo -leph -lsla -lm $(LIBNSL) \
	     -lcpgplot $(LPGPLOT)


############################################################################
# Here you should ONLY add programs which are ESSENTIAL for
# analyzing Parkes data.

ESSENTIAL_SRCS  = treduce.C tcedar.C archiver.C ar2scal.C plk.C plotcal.C \
                  tcombine2.C plk10.C ratio.C \

# These programs are not essential for analyzing Parkes data.
# Add inessential programs here
OTHER_SRCS      = align.C amps.C chanprep.C  \
                  mel.C newname.C oneprof.C plotsub.C    \
                  tadd.C calfbar.C modhead.C   \
		  plot_gry_diff.C pepoch.C stokes_plt.C freduce.C cal_arch.C \
		  cleanarch.C fixhdr.C feedmix.C crm.C  \
		  getcal.C crh.C psrhead.C autotadd.C \
		  bin_ph.C corflux.C flux.C fakedat.C quik_cal.C pdma.C \
		  fitmix.C 

NOT_WORKING     = bandpass.C detail.C pdma_fortran.f xtoa.C 

PROGRAMS = $(ESSENTIAL_SRCS) $(OTHER_SRCS)

include $(LOCAL_ROOT)/include/Makefile.extended




