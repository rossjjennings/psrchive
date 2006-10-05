
bin_PROGRAMS = psrplot paas pam pas pascii pat paz psradd psrdiff psrsh psrwt \
	fluxcal pac pacv pcm

psrplot_SOURCES = psrplot.C
psrplot_LDADD = $(LDADD) @PGPLOT_LIBS@

paas_SOURCES = paas.C
paas_LDADD = $(LDADD) @PGPLOT_LIBS@

pam_SOURCES = pam.C

pas_SOURCES = pas.C
pas_LDADD = $(LDADD) @PGPLOT_LIBS@

pascii_SOURCES = pascii.C
pat_SOURCES = pat.C

paz_SOURCES = paz.C zap_periodic_spikes.C

psradd_SOURCES = psradd.C

psrdiff_SOURCES = psrdiff.C
psrdiff_LDADD = $(LDADD) @PGPLOT_LIBS@

psrsh_SOURCES = psrsh.C
psrsh_LDADD = $(LDADD) @READLINE_LIBS@

psrwt_SOURCES = psrwt.C

fluxcal_SOURCES = fluxcal.C

pac_SOURCES = pac.C

pacv_SOURCES = pacv.C
pacv_LDADD = $(LDADD) @PGPLOT_LIBS@

pcm_SOURCES = pcm.C
pcm_LDADD = $(LDADD) @PGPLOT_LIBS@

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/include \
	@PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @MPI_CFLAGS@

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Core/libpsrcore.la \
        $(top_builddir)/Util/libpsrutil.la

include $(top_srcdir)/config/Makefile.include

