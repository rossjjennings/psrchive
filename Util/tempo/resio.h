#ifndef __RESIO_H
#define __RESIO_H

#ifdef __cplusplus
extern "C" {
#endif

  /* I/O functions to read from the resid2.tmp file produced by TEMPO */

  void resopen_ (int* lun, char* filename, int filename_length);
  
  void resread_ (int* lun, double* mjd, double* resiturns, double* resitimes,
		 double* phase, double* freq, double* weight, double* error,
		 double* preres, int* status);
  
  void resclose_ (int* lun);
  
#ifdef __cplusplus
}
#endif

#endif


