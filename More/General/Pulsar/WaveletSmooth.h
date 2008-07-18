//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_WaveletSmooth_h
#define __Pulsar_WaveletSmooth_h

#include "Pulsar/Transformation.h"
#include "Pulsar/WaveletTransform.h"

namespace Pulsar {

  class Profile;

  //! Profile smoothing (more accurately denoising) that uses a 
  //! thresholded wavelet transform.
  class WaveletSmooth : public Transformation<Profile> {

  public:

    //! Default constructor
    WaveletSmooth ();

    //! Destructor
    ~WaveletSmooth ();

    //! Get number of wavelet coeffs kept
    unsigned get_ncoeff() const { return ncoeff; };

    //! Smooth given Profile
    void transform(Profile *);

    //! Set wavelet type, order
    void set_wavelet(const gsl_wavelet_type *t, int order);

    //! Available threshold types
    enum Threshold { Hard, Soft };

    //! Set threshold method
    void set_threshold(Threshold t) { thresh = t; };

  protected:

    //! Number of coeffs kept
    int ncoeff;

    //! Threshold type
    Threshold thresh;

    //! Threshold cutoff
    double cutoff;

    //! Noise level in wavelet coeffs
    double sigma;

    //! The wavelet transform algorithm
    WaveletTransform wt;

  private:
    
    //! Hard thresholding
    double thresh_hard(double in);

    //! Soft thresholding
    double thresh_soft(double in);

  };

}

#endif
