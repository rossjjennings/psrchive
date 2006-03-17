/***************************************************************************
 *
 *   Copyright (C) 2001 by straten Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/*! This function takes the "halfcomplex" spectrum, as produced by rfftw_one:

   r0, r1, r2, ..., rn/2, i(n+1)/2-1, ..., i2, i1

   and sorts it into a complex spectrum:

   r0, rn/2, r1, i1, r2, i2, ..., rn/2-1, in/2-1

   Note that the DC and Nyquist samples (both real) are combined to
   form a new, complex sample, z = DC + i*Nyquist.  As the DC and
   Nyquist samples contain power from the edge of the band beyond the
   filter roll-off, this fact can be ignored in most cases.

   This new function should be used in place of the old rfftw_sort.
   However, in order to avoid backward compatibility problems, the old
   function is kept and the new is given a different name, rfftw_resort

   Willem van Straten, Dec 2003

*/

int rfftw_resort (int npts, float* halfcomplex_input, float* complex_output)
{
  int i;
  int npts2 = npts/2;

  complex_output[0] = halfcomplex_input[0];
  complex_output[1] = halfcomplex_input[npts2];

  if (npts%2)
    complex_output[npts-1] = halfcomplex_input[npts2+1];

  for (i=1; i<npts2; ++i) {
    complex_output[2*i] = halfcomplex_input[i];
    complex_output[2*i+1] = halfcomplex_input[npts-i];
  }

  return 0;
}

int rfftw_sort(int npts, float * src, float * dest)
{
  int i;
  dest[0] = src[0];
  dest[1] = 0.0;

  for(i=1; i<npts/2; ++i){
    dest[2*i] = src[i];
    dest[2*i+1] = src[npts-i];
  }

  dest[2*(npts/2)] = src[npts/2];

  if (npts%2==0)
    dest[npts+1] = 0.0;
  else
    dest[npts] = src[npts/2+1];

  return 0;
}

