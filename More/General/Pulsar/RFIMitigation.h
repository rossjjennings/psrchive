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
    void zap_chans (Pulsar::Archive* arch);
    
    
  private:
    
    // Set the weights array in a Pulsar::Integration to zap strong birdies
    vector<float> zap_mask (Pulsar::Integration* integ);
    
    // Apply a zap mask to an Integration
    void apply_mask (Pulsar::Integration* integ, vector<float> mask);
    
    // Initialise the class
    void init ();
    
  };
  
}

