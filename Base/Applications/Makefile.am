
bin_PROGRAMS = psredit psrtxt psrconv vip

psredit_SOURCES			= psredit.C
psrtxt_SOURCES			= psrtxt.C
psrconv_SOURCES			= psrconv.C
vip_SOURCES			= vip.C

#############################################################################

INCLUDES = \
	-I$(top_builddir)/Core/include \
	-I$(top_builddir)/utils/include

LDADD = $(top_builddir)/Core/libCore.la \
	$(top_builddir)/utils/libpsrutils.la

