
lib_LTLIBRARIES = libpsrchive.la

libpsrchive_la_SOURCES =

SUBDIRS = General

libpsrchive_la_LIBADD = General/libGeneral.la

# ######################################################################
#
# The following codes make use of MEAL
#

if !MAKE_LEAN

SUBDIRS += MEAL Polarimetry

libpsrchive_la_LIBADD += Polarimetry/libPolarimetry.la

if HAVE_PGPLOT
SUBDIRS += Plotting
libpsrchive_la_LIBADD += Plotter/libPlotting.la
endif

SUBDIRS += . Applications

if HAVE_QT
if HAVE_PGPLOT
SUBDIRS += GUI
endif
endif

if HAVE_QTDRIV
SUBDIRS += rhythm
endif

endif # !MAKE_LEAN

all: intermediate-install

include $(top_srcdir)/config/Makefile.local
