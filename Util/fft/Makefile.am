
noinst_LTLIBRARIES = libpsrfft.la

include_HEADERS = fftm.h psrfft.h spectra.h interpolate.h median_smooth.h \
           cross_detect.h stokes_detect.h differentiate.h BandpassPlotter.h \
	   FTransform.h IPP_Transform.h MKL_Transform.h FFTW3_Transform.h \
	   FFTW_Transform.h

libpsrfft_la_SOURCES = spectra.c noise.c running_median.c	 \
	local_medians.c optimize_fft.c fft_shift.c fftm.C correl.c	 \
	cdetect.c rfftw_sort.c cross_detect.c stokes_detect.c		 \
	fccf.f ffft.f differentiate.C fftfit.f fftconv.f brent.f cprof.f \
	cross_detect.ic stokes_detect.ic \
        FTransform.C IPP_Transform.C MKL_Transform.C FFTW3_Transform.C \
	FFTW_Transform.C

if HAVE_FFTW2
libpsrfft_la_SOURCES += psrfft.c
endif

TESTS = test_normalization test_interpolate test_real_complex

check_PROGRAMS = $(TESTS) test_optimize

test_normalization_SOURCES	= test_normalization.C
test_interpolate_SOURCES	= test_interpolate.C
test_interpolate_LDADD		= $(LDADD) ../units/libunits.la
test_real_complex_SOURCES	= test_real_complex.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/utils/units \
	-I$(top_srcdir)/utils/genutil @FFT_CFLAGS@

LDADD = libpsrfft.la ../units/libunits.la @FFT_LIBS@ 

libpsrfft_la_LIBADD = @FFT_LIBS@

