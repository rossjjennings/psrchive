#ifndef PSR_FFT_H
#define PSR_FFT_H

/* these all put :
  
   dest[0] = DC coeff
   dest[1] = 0.0
   ...
   dest[2*(npts-1)] = Nyq coeff
   dest[2*(npts-1)+1] = 0.0
*/

#ifdef __cplusplus
extern "C" {
#endif

extern int rcfft1d(int npts, float *dest, float *src);
extern int rcfft1dp(int npts, float *data);
extern int ccfft1d(int npts, float *dest, float *src);
extern int ccfft1dp(int npts, float *data);

#ifdef __cplusplus
}
#endif

#endif
