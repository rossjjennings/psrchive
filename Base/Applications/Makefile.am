
bin_PROGRAMS = psredit psrtxt psrconv vip vap

psredit_SOURCES			= psredit.C
psrtxt_SOURCES			= psrtxt.C
psrconv_SOURCES			= psrconv.C
vip_SOURCES			= vip.C
vap_SOURCES			= vap.C remap.C TextFinder.C VAPTI.C

#############################################################################

INCLUDES = -I$(top_builddir)/include

LDADD = $(top_builddir)/Base/libpsrbase.la \
	$(top_builddir)/Util/libpsrutil.la

