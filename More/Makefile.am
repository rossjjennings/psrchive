
SUBDIRS =

lib_LTLIBRARIES = libpsrmore.la
libpsrmore_la_LIBTOOLFLAGS = --tag=CXX
libpsrmore_la_SOURCES =
libpsrmore_la_LIBADD =

SUBDIRS += General
libpsrmore_la_LIBADD += General/libGeneral.la

# ######################################################################
#
# The following codes make use of MEAL
#

if !MAKE_LEAN

SUBDIRS += MEAL
libpsrmore_la_LIBADD += MEAL/libMEAL.la

SUBDIRS += Polarimetry
libpsrmore_la_LIBADD += Polarimetry/libPolarimetry.la

SUBDIRS += Timing
libpsrmore_la_LIBADD += Timing/libTiming.la

SUBDIRS += RFIMitigation
libpsrmore_la_LIBADD += RFIMitigation/libRFIMitigation.la

if HAVE_PGPLOT
SUBDIRS += Plotting
endif

SUBDIRS += Resources
libpsrmore_la_LIBADD += Resources/libResources.la

SUBDIRS += . Applications Simulation

if HAVE_QTDRIV
SUBDIRS += rhythm
endif

if HAVE_PGPLOT
if HAVE_QT
SUBDIRS += psrgui
endif
endif

endif # not MAKE_LEAN

if HAVE_PYTHON
SUBDIRS += python
endif

include $(top_srcdir)/config/Makefile.local

