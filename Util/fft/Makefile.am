
noinst_LTLIBRARIES = libpsrfft.la

include_HEADERS = interpolate.h median_smooth.h \
	FTransform.h FTransformAgent.h FTransformPlan.h \
	IPP_Transform.h MKL_Transform.h MKL_DFTI_Transform.h \
	FFTW3_Transform.h FFTW_Transform.h QuaternionFT.h

libpsrfft_la_SOURCES = test_libraries.h shift.C fccf.f ffft.f \
	FTransformAgent.C FTransform.C QuaternionFT.C


if HAVE_MKL_DFTI
libpsrfft_la_SOURCES += MKL_DFTI_Transform.C
endif

if HAVE_MKL
libpsrfft_la_SOURCES += MKL_Transform.C
endif

if HAVE_IPP
libpsrfft_la_SOURCES += IPP_Transform.C
endif

if HAVE_FFTW
libpsrfft_la_SOURCES += FFTW_Transform.C
endif

if HAVE_FFTW3
libpsrfft_la_SOURCES += FFTW3_Transform.C
endif

TESTS = test_frexp test_enum test_normalization test_interpolate \
	test_real_complex test_threads test_QuaternionFT

check_PROGRAMS = $(TESTS)

test_enum_SOURCES		= test_enum.C
test_normalization_SOURCES	= test_normalization.C
test_interpolate_SOURCES	= test_interpolate.C
test_interpolate_LDADD		= $(LDADD) ../units/libunits.la
test_real_complex_SOURCES	= test_real_complex.C
test_threads_SOURCES		= test_threads.C
test_QuaternionFT_SOURCES	= test_QuaternionFT.C

#############################################################################
#
# Dependencies should never be changed
#

INCLUDES = @FFT_CFLAGS@

LDADD = libpsrfft.la \
	$(top_builddir)/Util/genutil/libgenutil.la \
	$(top_builddir)/Util/units/libunits.la \
	$(top_builddir)/Util/third/libthird.la \
	@FFT_LIBS@ 

libpsrfft_la_LIBADD = @FFT_LIBS@

include $(top_srcdir)/config/Makefile.include

