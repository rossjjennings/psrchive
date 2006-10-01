
lib_LTLIBRARIES = libpsrmore.la

libpsrmore_la_SOURCES =

SUBDIRS = General
libpsrmore_la_LIBADD = General/libGeneral.la

# ######################################################################
#
# The following codes make use of MEAL
#

if !MAKE_LEAN
SUBDIRS += MEAL Polarimetry
libpsrmore_la_LIBADD += MEAL/libMEAL.la
libpsrmore_la_LIBADD += Polarimetry/libPolarimetry.la
endif #

if HAVE_PGPLOT
SUBDIRS += Plotting
libpsrmore_la_LIBADD += Plotting/libPlotting.la
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

all-local: intermediate-install

include $(top_srcdir)/config/Makefile.local

