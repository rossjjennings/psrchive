//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_StandardFlux_h
#define __Pulsar_StandardFlux_h

#include "Pulsar/Flux.h"
#include "Pulsar/ProfileShiftFit.h"
#include "Estimate.h"

namespace Pulsar
{
  class Profile;

  //! Computes average flux of a Profile by fitting to a standard.
  class StandardFlux : public Flux
  {   
  public:

    //! Default constructor
    StandardFlux();

    //! Destructor
    ~StandardFlux();

    //! Return flux w/ uncertainty
    Estimate<double> get_flux(const Profile *p);

    //! Return name of flux algorithm
    std::string get_method() const { return "StandardFlux"; }

    //! Set the standard profile to use
    void set_standard(const Profile *p);

  protected:

    //! Shift/scale fit
    ProfileShiftFit psf;

    //! Standard profile
    Reference::To<const Profile> stdprof;

    //! Std normalization factor
    double stdfac;

    //! Fit shift or just scale
    bool fit_shift;

  };

}

#endif
