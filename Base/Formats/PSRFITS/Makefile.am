
noinst_LTLIBRARIES = libpsrfits.la

include_HEADERS = Pulsar/FITSArchive.h

dist_data_DATA = psrheader.fits

libpsrfits_la_SOURCES = CalibratorExtensionIO.h FITSArchive.C \
	unload_FITSHdrExtension.C unload_ObsExtension.C \
	unload_Receiver.C load_Receiver.C \
	unload_WidebandCorrelator.C load_WidebandCorrelator.C \
	unload_CalInfoExtension.C load_CalInfoExtension.C \
	unload_ITRFExtension.C load_ITRFExtension.C \
	unload_ProcHistory.C load_ProcHistory.C \
	unload_DigitiserStatistics.C load_DigitiserStatistics.C \
	unload_CalibratorExtension.C load_CalibratorExtension.C \
	unload_PolnCalibratorExtension.C load_PolnCalibratorExtension.C \
	unload_FluxCalibratorExtension.C load_FluxCalibratorExtension.C \
	unload_CalibratorStokes.C load_CalibratorStokes.C \
	unload_Passband.C load_Passband.C \
	unload_Pointing.C load_Pointing.C \
	unload_Integration.C load_Integration.C \
	delete_hdu.C update_history.C unload_integrations.C \
	CalibratorExtensionIO.C FITSAgent.C

check_PROGRAMS = test_template
TESTS = $(check_PROGRAMS)

test_template_SOURCES = test_template.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir)/utils/include -I$(top_srcdir)/Core/Bases \
	-I$(top_srcdir)/psrchive/Polarimetry \
	-I$(top_srcdir)/Core/Extensions @CFITSIO_CFLAGS@ @MPI_CFLAGS@

LDADD = libpsrfits.la $(top_builddir)/utils/libpsrutils.la @CFITSIO_LIBS@

