#include "RFIMitigation.h"

// Destructor
Pulsar::RFIMitigation::~RFIMitigation () {
  // For now there is nothing to do...
}

// Run through an archive, setting the weights arrays in each subint
void Pulsar::RFIMitigation::zap_chans (Pulsar::Archive* arch)
{
  Pulsar::Archive* copy = arch->clone();
  copy->pscrunch();
  
  vector<float> mask(copy->get_nchan());
  
  for (unsigned i = 0; i < copy->get_nsubint(); i++) {
    mask = zap_mask(copy->get_Integration(i));
    apply_mask(arch->get_Integration(i), mask);
  }
}

// Set the weights array in a Pulsar::Integration to zap strong birdies
vector<float> Pulsar::RFIMitigation::zap_mask (Pulsar::Integration* integ) {
  
  int nchan = integ->get_nchan();
  
  // Define a vector to hold the bandpass
  vector<double> mean;
  mean.resize(nchan);
  
  float phase = 0.0;
  Pulsar::Profile* profile = 0;
  
  for (int i = 0; i < nchan; i++) {
    profile = integ->get_Profile(0, i);
    phase = profile->find_min_phase();
    mean[i] = profile->mean(phase);
  }
  
  float global_mean = 0.0;
  
  for (int i = 0; i < nchan; i++) {
    global_mean += mean[i];
  }
  
  global_mean = global_mean / float(nchan);
  
  // Define a vector to hold the mask
  vector<float> mask(nchan);
  
  for (int i = 0; i < nchan; i++) {
    if ( fabs(mean[i] - global_mean) > global_mean )
      mask[i] = 0.0;
    else
      mask[i] = 1.0;
  }
  
  return mask;
}

// Manually set specific channel weights to zero
void Pulsar::RFIMitigation::zap_specific (Pulsar::Archive* arch, vector<float> mask)
{
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    apply_mask(arch->get_Integration(i), mask);
  }
}

void Pulsar::RFIMitigation::apply_mask (Pulsar::Integration* integ, vector<float> mask)
{
  if (mask.size() != integ->get_nchan())
    throw Error(InvalidParam, "RFIMitigation::apply_mask incorrect length");
  
  for (unsigned i = 0; i < integ->get_nchan(); i++) {
    integ->get_Profile(0,i)->set_weight(mask[i]);
  }
}

void Pulsar::RFIMitigation::init () {
  // For now there is nothing to do...
}

