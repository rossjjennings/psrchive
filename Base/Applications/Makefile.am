
bin_PROGRAMS = psredit psrtxt psrconv vip

psredit_SOURCES			= psredit.C
psrtxt_SOURCES			= psrtxt.C
psrconv_SOURCES			= psrconv.C
vip_SOURCES			= vip.C

#############################################################################

INCLUDES = -I$(top_builddir)/utils/include \
	-I$(top_srcdir)/psrchive/File/Base \
	-I$(top_srcdir)/psrchive/File/Extension

LDADD = $(top_builddir)/psrchive/File/libFile.la \
	$(top_builddir)/utils/libpsrutils.la

