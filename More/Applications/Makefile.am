
bin_PROGRAMS = paas pam pas pascii pat paz pdmp psradd psrdiff psrsh psrwt

paas_SOURCES = paas.C
pam_SOURCES = pam.C
pas_SOURCES = pas.C
pascii_SOURCES = pascii.C
pat_SOURCES = pat.C
paz_SOURCES = paz.C
pdmp_SOURCES = pdmp.C
psradd_SOURCES = psradd.C
psrdiff_SOURCES = psrdiff.C
psrsh_SOURCES = psrsh.C
psrwt_SOURCES = psrwt.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = \
	-I$(top_builddir)/utils/include \
	-I$(top_builddir)/Core/include \
	-I$(top_builddir)/More/include \
	@PGPLOT_CFLAGS@ @CFITSIO_CFLAGS@ @MPI_CFLAGS@

LDADD = $(top_builddir)/More/libpsrmore.la \
	$(top_builddir)/Core/libpsrcore.la \
        $(top_builddir)/utils/libpsrutils.la

include $(top_srcdir)/config/Makefile.local

