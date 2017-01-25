//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BoostShiftAnalysis.h

#ifndef __Pulsar_BoostShiftAnalysis_h
#define __Pulsar_BoostShiftAnalysis_h

#include "Pulsar/PolnProfile.h"

namespace Pulsar {

  //! Analysis of the impact of instrumental boost on phase shift estimate
  class BoostShiftAnalysis {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    BoostShiftAnalysis () { max_harmonic = 0; }

    //! Set the maximum harmonic used when optimizing
    void set_profile (const PolnProfile*);

    //! Partial derivative of varphi with respect to b_k (beta=0)
    /*! Equation (26) of 1 November 2010 companion to van Straten (2006) */
    double delvarphi_delb (unsigned k) const;

    //! Three-vector of partial derivatives of varphi with respect to b_k
    Vector<3,double> dotvarphi () const;

    //! Partial derivative of varphi with respect to beta
    /*! Equation (27) of 1 November 2010 companion to van Straten (2006) */
    double delvarphi_delbeta () const;

  protected:

    //! The fourier transform of the polarization profile
    Reference::To<PolnProfile> fourier;

    //! The maximum harmonic to include in the analysis
    unsigned max_harmonic;

    //! Partial derivative of S_m with respect to b_k (beta=0)
    std::complex<double> delS_delb (unsigned k, unsigned m) const;

    //! Magnitude squared of S0 in mth harmonic
    double S0sq (unsigned m) const;

  };
}

#endif
