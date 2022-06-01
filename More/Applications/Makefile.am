
SUBDIRS = njkk08 .

noinst_PROGRAMS = example
example_SOURCES = example.C

bin_SCRIPTS = psrhelp

EXTRA_DIST = psrhelp

bin_PROGRAMS = pam pat paz psradd psrsh psrstat psrmodel psrwt \
	fluxcal pac pcm pdv rmfit mtm sesdiv psrstability psrflux \
	psrover psrcull calavg pcmavg fix_single psrsplit distortion \
	drifting_subpulses dynamic_spectra psr4th psrtxt2 reslice \
	smint psr4th2txt pcmrm pcmdiff pcmerr diffdelay

pam_SOURCES = pam.C

pat_SOURCES = pat.C
pat_LDADD = $(LDPLOT) @PTHREAD_LIBS@

paz_SOURCES = paz.C zap_periodic_spikes.C

calavg_SOURCES = calavg.C

pcmavg_SOURCES = pcmavg.C

pcmrm_SOURCES = pcmrm.C

pcmerr_SOURCES = pcmerr.C

diffdelay_SOURCES = diffdelay.C

fix_single_SOURCES = fix_single.C

psrsplit_SOURCES = psrsplit.C

psradd_SOURCES = psradd.C

psrflux_SOURCES = psrflux.C

psrsh_SOURCES = psrsh.C
psrsh_LDADD = $(LDADD) @READLINE_LIBS@

psrstability_SOURCES = psrstability.C

psrover_SOURCES = psrover.C

psrstat_SOURCES = psrstat.C

psrmodel_SOURCES = psrmodel.C
psrmodel_LDADD = $(LDPLOT)

psrwt_SOURCES = psrwt.C

fluxcal_SOURCES = fluxcal.C

pac_SOURCES = pac.C

# Note that pcm uses PGPLOT only if it is available
pcm_SOURCES = pcm.C
pcm_LDADD = $(LDPLOT) @PTHREAD_LIBS@

# Note that pdv uses PGPLOT only if it is available
pdv_SOURCES = pdv.C
pdv_LDADD = $(LDPLOT)

# Note that rmfit uses PGPLOT only if it is available
rmfit_SOURCES = rmfit.C

rmfit_LDADD = njkk08/libnjkk08.la $(LDPLOT)

mtm_SOURCES = mtm.C
distortion_SOURCES = distortion.C

drifting_subpulses_SOURCES = drifting_subpulses.C

dynamic_spectra_SOURCES = dynamic_spectra.C

sesdiv_SOURCES = sesdiv.C

psrcull_SOURCES = psrcull.C

psr4th_SOURCES = psr4th.C
psr4th2txt_SOURCES = psr4th2txt.C

psrtxt2_SOURCES = psrtxt2.C

reslice_SOURCES = reslice.C

smint_SOURCES = smint.C
smint_LDADD = @SPLINTER_LIBS@ $(LDPLOT)

pcmdiff_SOURCES = pcmdiff.C
pcmdiff_LDADD = $(LDPLOT)

######################################################################
#
# Eigen C++ template library specific code
#
if HAVE_EIGEN

bin_PROGRAMS += psrwhite
psrwhite_SOURCES = psrwhite.C

endif


# ######################################################################
#
# GSL specific code
#
if HAVE_GSL

bin_PROGRAMS += psrsmooth psrpca psrspa

psrsmooth_SOURCES = psrsmooth.C
psrsmooth_LDADD = $(LDADD) @GSL_LIBS@

psrpca_SOURCES = psrpca.C
psrpca_LDADD = $(LDADD) @GSL_LIBS@ @CULA_LIBS@

psrspa_SOURCES = psrspa.C
psrspa_LDADD = $(LDADD) @GSL_LIBS@

endif

# ######################################################################
#
# PGPLOT specific code
#
if HAVE_PGPLOT

bin_PROGRAMS += pdmp psrplot paas pas psrdiff pacv pazi psrzap pav \
	psrsp psrtrash spa lmfit pafit

lmfit_SOURCES = lmfit.C
lmfit_LDADD = $(LDPLOT)

pafit_SOURCES = pafit.C 
pafit_LDADD = $(LDPLOT)

pdmp_SOURCES = pdmp.C
pdmp_LDADD = $(LDPLOT) @PSRXML_LIBS@

psrplot_SOURCES = psrplot.C
psrplot_LDADD = $(LDPLOT)

paas_SOURCES = paas.C
paas_LDADD = $(LDPLOT)

pas_SOURCES = pas.C
pas_LDADD = $(LDPLOT)

spa_SOURCES = spa.C
spa_LDADD = $(LDPLOT)

psrdiff_SOURCES = psrdiff.C
psrdiff_LDADD = $(LDPLOT)

pacv_SOURCES = pacv.C
pacv_LDADD = $(LDPLOT)

pazi_SOURCES = pazi.C
pazi_LDADD = $(LDPLOT)

psrzap_SOURCES = psrzap.C
psrzap_LDADD = $(LDPLOT)

pav_SOURCES = pav.C PavApp.C PavApp.h
pav_LDADD = $(LDPLOT)

psrsp_SOURCES = psrsp.C
psrsp_LDADD = $(LDPLOT)

psrtrash_SOURCES = psrtrash.C
psrtrash_LDADD = $(LDPLOT)

LDPLOT = $(top_builddir)/More/Plotting/libpsrplot.la $(LDADD) @PGPLOT_LIBS@

else

LDPLOT = $(LDADD)
endif

# ######################################################################
#
# HEALPix specific code
#
if HAVE_HEALPIX

bin_PROGRAMS += psrpol

psrpol_SOURCES = psrpol.C
psrpol_LDADD = $(LDADD) @HEALPIX_LIBS@ @CFITSIO_LIBS@

endif

TESTS = test_ProfileStatistic_factory

check_PROGRAMS = $(TESTS) print_Predictor

test_ProfileStatistic_factory_SOURCES = test_ProfileStatistic_factory.C
print_Predictor_SOURCES = print_Predictor.C

#############################################################################
#

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la \
	$(top_builddir)/Util/epsic/src/libepsic.la

include $(top_srcdir)/config/Makefile.include

AM_CPPFLAGS += @PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @GSL_CFLAGS@ @PSRXML_CFLAGS@ @CULA_CFLAGS@ @HEALPIX_CFLAGS@

