
bin_SCRIPTS = psrhelp

bin_PROGRAMS = pam pascii pat paz psradd psrsh psrwt fluxcal pac pcm

pam_SOURCES = pam.C

pascii_SOURCES = pascii.C
pat_SOURCES = pat.C

paz_SOURCES = paz.C zap_periodic_spikes.C

psradd_SOURCES = psradd.C

psrsh_SOURCES = psrsh.C
psrsh_LDADD = $(LDADD) @READLINE_LIBS@

psrwt_SOURCES = psrwt.C

fluxcal_SOURCES = fluxcal.C

pac_SOURCES = pac.C

# Note that pcm uses PGPLOT only if it is available
pcm_SOURCES = pcm.C
pcm_LDADD = $(LDADD) @PGPLOT_LIBS@ @PTHREAD_LIBS@

# ######################################################################
#
# PGPLOT specific code
#
if HAVE_PGPLOT

bin_PROGRAMS += psrplot paas pas psrdiff pacv

psrplot_SOURCES = psrplot.C
psrplot_LDADD = $(LDADD) @PGPLOT_LIBS@

paas_SOURCES = paas.C
paas_LDADD = $(LDADD) @PGPLOT_LIBS@

pas_SOURCES = pas.C
pas_LDADD = $(LDADD) @PGPLOT_LIBS@

psrdiff_SOURCES = psrdiff.C
psrdiff_LDADD = $(LDADD) @PGPLOT_LIBS@

pacv_SOURCES = pacv.C
pacv_LDADD = $(LDADD) @PGPLOT_LIBS@

endif

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = @PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Base/libpsrbase.la \
        $(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

