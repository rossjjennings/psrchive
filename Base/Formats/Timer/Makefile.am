LOCAL_ROOT = ../../..
include ${LOCAL_ROOT}/include/Makefile.standard

INCLUDES = archive.h archhdr.h headers.h mini++.h timer++.h \
	timer.h mini.h band.h \
	timer.inc band.inc mini.inc convert_endian.h misc.h

LIBRARY    = libarchive.a
LIB_C_SRCS = pg_plot_gnu.C profile.C sub_int.C archive.C superarchive.C  \
             toa.C profplot.C plot2profs.C scintplot.C misc.C heapsort.C \
             get_epoch_and_name.C archhdr.C convert_endian.c headers.C \
             timer++.C mini++.C

LIB_F_SRCS = ffft.f fftfit.f brent.f fccf.f cprof.f shiftbyfft.f \
             width.f tsky1.f fftconv.f scan_fort_val.f convolve.f

LIB_SRCS   = ${LIB_C_SRCS} ${LIB_F_SRCS}

LDFLAGS   += -larchive -lpolyco ${LDIALOG} -ltimes \
             -lpsrinfo -leph -lgenutil -lsla $(LFFT) -lm $(LIBNSL) \
	     -lcpgplot $(LPGPLOT) 

treduce.srcs = xtinit.C xtmac.C xtproc.C xtlplt.C xtinplot.C xtshdr.C    \
	       xtshst.C xtcopy.C xtswap.C xtunld.C xtgrey.C xtreduce.C   \
               newtreduce.f

PROGRAMS =    calinfo.C align.C amps.C chanprep.C  \
	      mel.C newname.C oneprof.C plotsub.C    \
	      tadd.C calfbar.C modhead.C   \
	      plot_gry_diff.C freduce.C \
	      cleanarch.C fixhdr.C crm.C  \
	      crh.C psrhead.C autotadd.C \
	      fakedat.C plk.C stokes_plt.C \
	      headsum.C treduce.C \
              tcombine2.C ratio.C fixprf.C xformprof.C archive_test.C

NOT_WORKING = bandpass.C detail.C xtoa.C bin_ph.C corflux.C flux.C \
	vargain.C pepoch.C wtplt.C

include $(LOCAL_ROOT)/include/Makefile.extended

