
noinst_LTLIBRARIES = libpsrfft.la

include_HEADERS = fftm.h psrfft.h spectra.h interpolate.h median_smooth.h \
           cross_detect.h stokes_detect.h differentiate.h BandpassPlotter.h

libpsrfft_la_SOURCES = spectra.c psrfft.c noise.c running_median.c	 \
	local_medians.c optimize_fft.c fft_shift.c fftm.C correl.c	 \
	cdetect.c rfftw_sort.c cross_detect.c stokes_detect.c		 \
	fccf.f ffft.f differentiate.C fftfit.f fftconv.f brent.f cprof.f \
	cross_detect.ic stokes_detect.ic

bin_PROGRAMS = test_optimize

TESTS = test_normalization test_interpolate

check_PROGRAMS = $(TESTS)

test_normalization_SOURCES	= test_normalization.C
test_interpolate_SOURCES	= test_interpolate.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_srcdir) -I../genutil

LDADD = libpsrfft.la @FFT_LIBS@ 

