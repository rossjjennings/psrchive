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
#include "Pulsar/ProfileShiftFit.h"

namespace Pulsar {

  //! Estimates phase shift in Fourier domain with MCMC error estimate */
  class FourierDomainFit : public ProfileStandardShift
  {

  public:

    FourierDomainFit ();

    //! Set the template profile
    void set_standard (const Profile* p);

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
    void set_mcmc (bool flag = true) { error_method = flag ? "mcmc" : "trad"; }
    bool get_mcmc () const { return error_method=="mcmc"; }

    //! Set number of iterations for MCMC
    void set_iterations (int nit) { fit.set_mcmc_iterations(nit); }
    int get_iterations () const { return fit.get_mcmc_iterations(); }

    //! Set uncertainty calculation method
    void set_error_method (std::string m) { error_method=m; }
    std::string get_error_method () const { return error_method; }

  protected:

    class Interface;

    //! The uncertainty calculation method
    std::string error_method;

    //! Reduced chisq of last call to get_shift
    mutable double reduced_chisq;

    //! S/N ratio of last profile fit
    mutable double snr;

    //! The class that does the actual fit
    mutable ProfileShiftFit fit;

  };

}


#endif // !defined __Pulsar_FourierDomainFit_h
