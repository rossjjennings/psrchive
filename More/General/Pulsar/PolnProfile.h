#ifndef __PolnProfile_h
#define __PolnProfile_h

#include "Reference.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

#include "Error.h"

//#include "Stokes.h"

namespace Pulsar {

  class PolnProfile : public Reference::Able {
    
  public:
    
    // Clean slate
    void init ();
    // Null constructor
    PolnProfile () { init(); }
    // Construct from an Integration
    PolnProfile (const Integration& subint);
    // Construct from four profiles
    PolnProfile (Profile* zero, Profile* one,
		 Profile* two, Profile* three);
    
    // Destructor
    ~PolnProfile();
    
    // Access functions
    //Stokes<float> get_amps (unsigned ibin);
    
    Signal::State get_state () { return state; }
    
    // Manipulation functions
    void transform (Jones<float>& response);
    void convert_state (Signal::State);
    
  protected:

    void set_amps (unsigned ibin, const Stokes<float>& new_amps);
    
    Signal::State state;
    unsigned nbin;
    vector<(Profile*> data;
    
  };


}

#endif



