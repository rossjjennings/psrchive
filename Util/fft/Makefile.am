
noinst_LTLIBRARIES = libpsrfft.la

include_HEADERS = interpolate.h median_smooth.h \
	   FTransform.h IPP_Transform.h MKL_Transform.h FFTW3_Transform.h \
	   FFTW_Transform.h QuaternionFT.h

libpsrfft_la_SOURCES = \
	FTransform.C IPP_Transform.C MKL_Transform.C FFTW3_Transform.C \
	FFTW_Transform.C QuaternionFT.C shift.C fccf.f ffft.f

TESTS = test_normalization test_interpolate test_real_complex \
	test_QuaternionFT

check_PROGRAMS = $(TESTS)

test_normalization_SOURCES	= test_normalization.C
test_interpolate_SOURCES	= test_interpolate.C
test_interpolate_LDADD		= $(LDADD) ../units/libunits.la
test_real_complex_SOURCES	= test_real_complex.C
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

