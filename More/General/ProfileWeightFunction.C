#include "Pulsar/ProfileWeightFunction.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

//! Set the Profile from which the PhaseWeight will be derived
void Pulsar::ProfileWeightFunction::set_Profile (const Profile* _profile)
{
  profile = _profile;
}

//! Returns a PhaseWeight with the Profile attribute set
void Pulsar::ProfileWeightFunction::get_weight (PhaseWeight& weight)
{
  calculate (weight);
  weight.set_Profile (profile);
}


