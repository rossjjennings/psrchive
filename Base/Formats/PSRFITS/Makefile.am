
noinst_LTLIBRARIES = libpsrfits.la

include_HEADERS = Pulsar/FITSArchive.h

libpsrfits_la_SOURCES = CalibratorExtensionIO.h FITSArchive.C \
	unload_FITSHdrExtension unload_ObsExtension \
	unload_Receiver load_Receiver \
	unload_WidebandCorrelator load_WidebandCorrelator \
	unload_CalInfoExtension load_CalInfoExtension \
	unload_ITRFExtension load_ITRFExtension \
	unload_ProcHistory load_ProcHistory \
	unload_DigitiserStatistics load_DigitiserStatistics \
	unload_CalibratorExtension load_CalibratorExtension \
	unload_PolnCalibratorExtension load_PolnCalibratorExtension \
	unload_FluxCalibratorExtension load_FluxCalibratorExtension \
	unload_CalibratorStokes load_CalibratorStokes \
	unload_Passband load_Passband \
	unload_Pointing load_Pointing \
	unload_Integration load_Integration \
	delete_hdu.C update_history.C unload_integrations.C \
	CalibratorExtensionIO.C Agent.C

check_PROGRAMS = test_template

test_template_SOURCES = test_template.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_srcdir)/utils/include -I$(top_srcdir)/astro/include \
        -I.. -I../Calibration -I../Extension

LDADD = $(top_srcdir)/utils/libpsrutils.la

