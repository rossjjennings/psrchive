#include <iostream>

#include <vector>
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "math.h"

void Pulsar::Integration::get_PA (vector<double> &phases, vector<double> &angles, vector<double> &errors, float threshold)
{

  Signal::State state = get_state();
  Reference::To<Pulsar::Integration> data = clone();

  if (state != Signal::Stokes) {
    data->convert_state(Signal::Stokes);
  }

  data->fscrunch();
 
  const Profile* Q = data->get_Profile (1,0);
  const Profile* U = data->get_Profile (2,0);
  const float* q = Q->get_amps();
  const float* u = U->get_amps();
  unsigned nbin = Q->get_nbin();

  vector< Estimate<float> > position_angle (nbin);

  vector<float> linear (nbin);

  for (unsigned ibin=0; ibin<nbin; ibin++) {

    linear[ibin] = sqrt (q[ibin]*q[ibin] + u[ibin]*u[ibin]);
    position_angle[ibin].val = (90 + 90.0/M_PI * atan2 (u[ibin], q[ibin]));
  }

  Profile linear_profile;
  linear_profile.set_amps (linear);

  linear_profile -= linear_profile.mean (linear_profile.find_min_phase());
 
  double min_avg,min_var;
  linear_profile.stats(linear_profile.find_min_phase(),&min_avg,&min_var);
  for (unsigned ibin=0;ibin<nbin;ibin++) {
    position_angle[ibin].var = 0.5 * ( sqrt(min_var)/(linear_profile.get_amps()[ibin]));
    position_angle[ibin].var *=180/M_PI;
  }

  phases.resize(0);
  angles.resize(0);
  errors.resize(0);

  for (unsigned ibin=0;ibin<nbin;ibin++) {
    if (linear_profile.get_amps()[ibin] > threshold*(sqrt(min_var))) {
      phases.push_back((float) fmod ((double)ibin/(double)nbin, 1.0));
      angles.push_back(position_angle[ibin].val);
      errors.push_back(position_angle[ibin].var);
    }
  }
}
