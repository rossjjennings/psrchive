//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Timing/Pulsar/PhaseGradShift.h

#ifndef __Pulsar_PhaseGradShift_h
#define __Pulsar_PhaseGradShift_h

#include "Pulsar/ProfileStandardShift.h"
#include "Pulsar/Config.h"

namespace Pulsar {

  class ScalarTemplateMatching;

  //! Estimates phase shift in Fourier domain
  class PhaseGradShift : public ProfileStandardShift
  {
    //! Interface to the model_profile algorithm
    Reference::To<ScalarTemplateMatching> stm;

  public:

    //! Compute the reduced chisq, or assume that it is unity
    static Option<bool> compute_reduced_chisq;

    //! Default constructor
    PhaseGradShift();

    //! Destructor
    ~PhaseGradShift();

    //! Set the profile with respect to which the shift will be estimated
    void set_standard (const Profile* p);

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Allow software to choose the maximum harmonic
    void set_choose_maximum_harmonic (bool flag = true);
    
    //! Return the best-fit estimate of the phase shift in turns
    Estimate<double> get_shift () const;

    //! Return the statistical goodness of fit
    double get_reduced_chisq () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ()
    { return new EmptyInterface<PhaseGradShift> ("PGS"); }

    //! Return a copy constructed instance of self
    PhaseGradShift* clone () const { return new PhaseGradShift(*this); }

  };

}


#endif // !defined __Pulsar_PhaseGradShift_h
