/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

namespace Pulsar {
  
  class RFIMitigation {
    
  public:
    
    // Null constructor
    RFIMitigation () { init(); }
    
    // Destructor
    ~RFIMitigation ();
    
    // Run through an archive, setting the weights arrays in each subint
    // according to an automated bad point detection algorithm
    void zap_chans (Pulsar::Archive* arch);
    
    // Manually set specific channel weights to zero
    void zap_specific (Pulsar::Archive* arch, vector<float> mask);

    // Manually set specific channel weights in specific subints to zero
    void zap_very_specific (Pulsar::Archive* arch, vector<float> mask,
			    vector<unsigned> subs);
    
  private:
    
    // Set the weights array in a Pulsar::Integration to zap strong birdies
    vector<float> zap_mask (Pulsar::Integration* integ);
    
    // Apply a zap mask to a Pulsar::Integration
    void apply_mask (Pulsar::Integration* integ, vector<float> mask);
    
    // Initialise the class
    void init ();
    
  };
  
}

