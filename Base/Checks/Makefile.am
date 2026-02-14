
noinst_LTLIBRARIES = libChecks.la

nobase_dist_data_DATA = fluxcal.on fluxcal.off \
	Parkes/receiver.log Parkes/MULT_1.rcvr \
	Parkes/H-OH.rcvr Parkes/1050CM.rcvr Parkes/5010CM.rcvr Parkes/UWL.rcvr

nobase_include_HEADERS = \
        Pulsar/CalSource.h \
	Pulsar/DispersionCheck.h \
	Pulsar/BirefringenceCheck.h \
	Pulsar/SetReceiver.h

libChecks_la_SOURCES = Check_registry.C \
	CalSource.C \
	DispersionCheck.C \
	BirefringenceCheck.C \
	SetReceiver.C

#############################################################################
#

include $(top_srcdir)/config/Makefile.include

