#ifndef PSR_FFT_H
#define PSR_FFT_H

#ifdef __cplusplus
extern "C" {
#endif

/* redwards 19 Aug 99 NOTE!! FFTW backward complex->real obliterates
   its input array! Hence I have only provided an in-place version --
   don't want to go destroying input but nor do I want to go allocating
   memory unexpectedly.
   */

/* forward FFTs */
int frcfft1d(int npts, float *dest, float *src);
int fccfft1d(int npts, float *dest, float *src);
/* backward FFTs */
int bccfft1d(int npts, float *dest, float *src);
/* in place versions */
int frcfft1dp(int npts, float *data);
int bccfft1dp(int npts, float *data);
int bcrfft1dp(int npts, float *dest);

int rfftw_sort(int npts, float *src, float *dest);

#ifdef __cplusplus
}
#endif

#endif
