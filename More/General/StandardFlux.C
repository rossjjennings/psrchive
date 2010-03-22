/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardFlux.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfileShiftFit.h"
#include "Pulsar/PhaseWeight.h"

#include <math.h>

Pulsar::StandardFlux::StandardFlux() 
{
  stdprof = NULL;
  stdfac = 0.0;
  fit_shift = true;
}

Pulsar::StandardFlux::~StandardFlux() 
{
}

void Pulsar::StandardFlux::set_standard(const Profile *p) 
{
  stdprof = p;
  psf.set_standard(const_cast<Profile *>(stdprof.get()));
  psf.set_nharm(stdprof->get_nbin()/4);
  psf.set_error_method(ProfileShiftFit::Traditional_Chi2);
  Reference::To<PhaseWeight> base = stdprof->baseline();
  stdfac = stdprof->sum()/(double)stdprof->get_nbin() 
    - base->get_mean().get_value();
}

Estimate<double> Pulsar::StandardFlux::get_flux(const Profile *p)
{
  psf.set_Profile(const_cast<Profile *>(p));
  return psf.get_scale() * stdfac;
}
