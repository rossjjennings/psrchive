//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/fft/interpolate.h,v $
   $Revision: 1.6 $
   $Date: 2004/10/26 12:54:37 $
   $Author: straten $*/

#ifndef __fft_interpolate_h
#define __fft_interpolate_h

#include "Traits.h"
#include "Error.h"
#include "fftm.h"

#include <vector>

namespace fft {

  extern bool verbose;

  template <class T>
  void interpolate (vector<T>& out, const vector<T>& in) {
    
    if (in.size() >= out.size())
      throw Error (InvalidParam, "fft::interpolate",
		   "in.size=%d >= out.size=%d.  just scrunch",
		   in.size() >= out.size());
    
    vector< std::complex<float> > dom1 (out.size());
    vector< std::complex<float> > dom2 (out.size());
    
    DatumTraits<T> datum_traits;

    unsigned ndim = datum_traits.ndim();
    
    if (verbose)
      cerr << "fft::interpolate " << ndim << " dimensions from "
	   << in.size() << " to " << out.size() << endl;
    
    unsigned ipt, idim;

    // for each dimension of the type T, perform an interpolation
    for (idim=0; idim<ndim; idim++) {

      for (ipt=0; ipt < in.size(); ipt++)
	dom1[ipt] = datum_traits.element (in[ipt], idim);

      fft::fcc1d (in.size(), (float*)&(dom2[0]), (float*)&(dom1[0]));

      // zero pad the rest
      for (ipt=in.size(); ipt<dom2.size(); ipt++)
	dom2[ipt] = 0;

      fft::bcc1d (out.size(), (float*)&(dom1[0]), (float*)&(dom2[0]));

      // this factor may need to be carefully chosen, depending
      // on how the FFT routines operate
      float factor = in.size();

      for (ipt=0; ipt < out.size(); ipt++)
	datum_traits.element (out[ipt], idim) = 
	  datum_traits.element_traits.cast (dom1[ipt*2]/factor);

    } // end for each dimension

  } // end of interpolate

} // end of namespace fft

#endif // ! __fft_interpolate_h
