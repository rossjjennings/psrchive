//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/fft/interpolate.h,v $
   $Revision: 1.4 $
   $Date: 2001/08/09 06:42:59 $
   $Author: straten $*/

#ifndef __fft_interpolate_h
#define __fft_interpolate_h

#include <string>
#include "fftm.h"

namespace fft {

  extern bool verbose;

  template <class T>
    void interpolate (vector<T>& out, const vector<T>& in) {
    
    if (in.size() > out.size())
      throw string ("fft::interpolate out smaller than in.  just scrunch");

    vector<float> dom1 (out.size()*2);
    vector<float> dom2 (out.size()*2);

    unsigned ndim = T::ndim;
    
    if (verbose)
      cerr << "fft::interpolate " << ndim << " dimensions from "
	   << in.size() << " to " << out.size() << endl;
    
    unsigned ipt, idim;

    // for each dimension of the type T, perform an interpolation
    for (idim=0; idim<ndim; idim++) {

      for (ipt=0; ipt < in.size(); ipt++)
	dom1[ipt] = in[ipt][idim];

      fft::frc1d (in.size(), dom2.begin(), dom1.begin());

      unsigned end = dom2.size()-2;
      // copy the conjugate of the transform, backwards, into the
      // end of the array, excluding DC
      for (ipt=2; ipt < in.size(); /* incr in loop */) {
	dom2[end]   = dom2[ipt];   // Re
	ipt ++;
	dom2[end+1] = -dom2[ipt];  // Im
	ipt ++;
	end -= 2;
      }

      // zero pad the rest
      unsigned stop = dom2.size()-in.size()+2;
      for (ipt=in.size(); ipt<stop; ipt++)
	dom2[ipt] = 0;

      fft::bcc1d (out.size(), dom1.begin(), dom2.begin());

      // this factor may need to be carefully chosen, depending
      // on how the FFT routines operate
      float factor = in.size();

      for (ipt=0; ipt < out.size(); ipt++)
	out[ipt][idim] = dom1[ipt*2]/factor;

    } // end for each dimension

  } // end of interpolate

} // end of namespace fft

#endif // ! __fft_interpolate_h
