
noinst_LTLIBRARIES = libpsrfft.la

include_HEADERS = interpolate.h median_smooth.h \
	   FTransform.h IPP_Transform.h MKL_Transform.h FFTW3_Transform.h \
	   FFTW_Transform.h QuaternionFT.h

libpsrfft_la_SOURCES = \
        FTransform.C IPP_Transform.C MKL_Transform.C FFTW3_Transform.C \
	FFTW_Transform.C QuaternionFT.C

TESTS = test_normalization test_interpolate test_real_complex \
	test_QuaternionFT test_rfftw_sort

check_PROGRAMS = $(TESTS)

test_normalization_SOURCES	= test_normalization.C
test_interpolate_SOURCES	= test_interpolate.C
test_interpolate_LDADD		= $(LDADD) ../units/libunits.la
test_real_complex_SOURCES	= test_real_complex.C
test_rfftw_sort_SOURCES		= test_rfftw_sort.C
test_QuaternionFT_SOURCES	= test_QuaternionFT.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = -I$(top_builddir) -I$(top_srcdir)/utils/units \
	-I$(top_srcdir)/utils/genutil -I$(top_builddir)/utils/units \
	@FFT_CFLAGS@

LDADD = libpsrfft.la ../units/libunits.la @FFT_LIBS@ 

libpsrfft_la_LIBADD = @FFT_LIBS@

