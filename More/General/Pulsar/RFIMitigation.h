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
    void zap_chans (Pulsar::Integration* integ);
    
    void init ();
    
  };
  
}

