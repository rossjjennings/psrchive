#include "RFIMitigation.h"

// Destructor
Pulsar::RFIMitigation::~RFIMitigation () {
  // For now there is nothing to do...
}

// Run through an archive, setting the weights arrays in each subint
void Pulsar::RFIMitigation::zap_chans (Pulsar::Archive* arch)
{
  for (unsigned i = 0; i < arch->get_nsubint(); i++) {
    zap_chans(arch->get_Integration(i));
  }
}

// Set the weights array in a Pulsar::Integration to zap strong birdies
void Pulsar::RFIMitigation::zap_chans (Pulsar::Integration* integ) {

  //Code goes here...

}

void init () {
  // For now there is nothing to do...
}

