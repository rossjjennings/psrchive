//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Wavelet_h
#define __Pulsar_Wavelet_h

#include "ReferenceAble.h"

#include <gsl/gsl_wavelet.h>

namespace Pulsar {

  class Profile;

  //! Performs 1-D discrete wavelet transforms (DWT)
  /*! Performs 1-D discrete wavelet transforms (DWT).  This class
   * is basically a wrapper for the GSL wavelet implementation.
   * Refer to GSL documentation for more info. 
   */
  class WaveletTransform : public Reference::Able {

    public:

      //! Constructor
      WaveletTransform();

      //! Destructor
      ~WaveletTransform();

      //! Set wavelet type
      void set_type(gsl_wavelet_type *t) { type=t; };

      //! Set wavelet order
      void set_order(int o) { order=o; };

      //! Perform forward transform
      void transform(unsigned npts, const float *in);

      //! Perform forward transform
      void transform(const std::vector<float>& in);

      //! Perform forward transform
      void transform(const Pulsar::Profile* in);

      //! Perform inverse transform in-place
      void invert();

      //! Possible states of the data
      enum State { Wavelet, Time, Empty };

      //! Get current state
      State get_state() { return state; }

      //! Get wavelet data
      double get_data(int level, int k);

      //! Get data element
      double get_data(int n);

    protected:

      //! Wavelet type
      const gsl_wavelet_type *type;

      //! Wavelet order
      int order;

      //! Wavelet coeffs
      gsl_wavelet *wave;

      //! Number of points
      int npts;

      //! Log2 of npts
      int log2_npts;

      //! Data storage
      double *data;

      //! Current state of the data
      State state;

      //! GSL workspace
      gsl_wavelet_workspace *work;

  };

}

#endif
