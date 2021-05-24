/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/third/gcvspl/slalib.h

#ifndef __GCVSPL_DECL_H
#define __GCVSPL_DECL_H

/*
  This C wrapper allows PSRCHIVE software to link against the Fortran implementation of GCVSPL.
*/

#ifdef __cplusplus
extern "C" {
#endif

  int gcvspl (double *x, double *y, int ny, 
	      double *wx, double *wy, int m, int n, int k, 
	      int md, double val, double *c, int nc, 
	      double *wk);

  double splder ( int ider, int m, int n, double t,
		  double *x, double *c__, int *l, double *q );

#ifdef __cplusplus
           }
#endif

#endif

