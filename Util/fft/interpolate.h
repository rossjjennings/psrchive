//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2001 - 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/fft/interpolate.h

#ifndef __fft_interpolate_h
#define __fft_interpolate_h

#include "Traits.h"
#include "Error.h"

#include <vector>
#include <iostream>

namespace fft {

  //! Use zero-padded Fourier transform to interpolate between samples
  void interpolate (std::complex<float>* dest, unsigned n_dest, const std::complex<float>* src, unsigned n_src);

  //! Use zero-padded Fourier transform to interpolate between samples of multi-dimensional structure
  template <class T>
  void interpolate (std::vector<T>& out, const std::vector<T>& in, bool verbose = false)
  {
    if (in.size() >= out.size())
      throw Error (InvalidParam, "fft::interpolate",
		   "in.size=%d >= out.size=%d.  just scrunch",
		   in.size() >= out.size());
    
    std::vector< std::complex<float> > dom1 (in.size());
    std::vector< std::complex<float> > dom2 (out.size());

    DatumTraits<T> datum_traits;

    unsigned ndim = datum_traits.ndim();
    if (verbose)
      std::cerr << "fft::interpolate ndim=" << ndim << " in.size=" << in.size() << " out.size=" << out.size() << std::endl;

    unsigned ipt = 0;

    // for each dimension of the type T, perform an interpolation
    for (unsigned idim=0; idim<ndim; idim++)
    {
      for (ipt=0; ipt < in.size(); ipt++)
	      dom1[ipt] = datum_traits.element (in[ipt], idim);

      interpolate (dom2.data(), dom2.size(), dom1.data(), dom1.size());

      for (ipt=0; ipt < out.size(); ipt++)
	      datum_traits.element (out[ipt], idim) = datum_traits.element_traits.from_complex (dom2[ipt]);

    } // end for each dimension

  } // end of interpolate

} // end of namespace fft

#endif // ! __fft_interpolate_h
