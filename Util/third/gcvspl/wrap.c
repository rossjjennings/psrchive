/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This file provides a C wrapper for the GCVSPL and SPLDER functions
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gcvspl.h"

/* *********************************************************************** */

#define f77gcvspl F77_FUNC_(gcvspl,gcvspl)

int f77gcvspl (double *x, double *y, int *ny, 
	       double *wx, double *wy, int *m, int *n, int *k, 
	       int *md, double *val, double *c, int *nc, 
	       double *wk, int *ier);

int gcvspl (double *x, double *y, int ny, 
	    double *wx, double *wy, int m, int n, int k, 
	    int md, double val, double *c, int nc, 
	    double *wk)
{
  int ier = 0;
  f77gcvspl ( x, y, &ny, wx, wy, &m, &n, &k, &md, &val, c, &n, wk, &ier );
  return ier;
}

#define f77splder F77_FUNC_(splder,splder)

double f77splder (int* ider, int* m, int* n, double* t,
		  double *x, double *c, int *l, double *q);
  
double splder (int ider, int m, int n, double t,
	       double *x, double *c, int *l, double *q)
{
  return f77splder ( &ider, &m, &n, &t, x, c, l, q );
}
