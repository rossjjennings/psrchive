//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/FourierDomainFit.h

#ifndef __Pulsar_FourierDomainFit_h
#define __Pulsar_FourierDomainFit_h

#include "Pulsar/ProfileStandardShift.h"

namespace Pulsar {

  //! Estimates phase shift in Fourier domain with MCMC error estimate */
  class FourierDomainFit : public ProfileStandardShift
  {

  public:

    FourierDomainFit ();

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return the statistical goodness-of-fit
    double get_reduced_chisq () const;

    //! Return the profile S/N ratio (relative to the standard)
    double get_snr () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Return a copy constructed instance of self
    FourierDomainFit* clone () const { return new FourierDomainFit(*this); }

    //! Use Markov Chain Monte Carlo method to determine TOA uncertainty
    void set_mcmc (bool flag = true) { use_mcmc = flag; }
    bool get_mcmc () const { return use_mcmc; }

  protected:

    class Interface;

    //! Use Markov Chain Monte Carlo method to determine TOA uncertainty
    bool use_mcmc;

    //! Reduced chisq of last call to get_shift
    mutable double reduced_chisq;

    //! S/N ratio of last profile fit
    mutable double snr;

  };

}


#endif // !defined __Pulsar_FourierDomainFit_h
