#ifndef __PolnProfile_h
#define __PolnProfile_h

#include "Reference.h"
#include "Stokes.h"
#include "Jones.h"
#include "Types.h"

namespace Pulsar {

  class Profile;

  class PolnProfile : public Reference::Able {
    
  public:
    //! Null constructor
    PolnProfile () { init(); }

    //! Construct from four profiles
    PolnProfile (Signal::Basis basis, Signal::State state,
		 Profile* p0, Profile* p1,
		 Profile* p2, Profile* p3, bool clone = false);
    
    //! Destructor
    ~PolnProfile();
    
    //! Get the Stokes 4-vector for the specified bin
    Stokes<float> get_Stokes (unsigned ibin);

    //! Set the Stokes 4-vector for the specified bin
    void set_Stokes (unsigned ibin, const Stokes<float>& stokes);

    //! Get the State of the poln profile
    Signal::State get_state () const { return state; }

    //! Get pointers to the Profile instances
    void get_Profiles (Profile* &p0, Profile* &p1,
		       Profile* &p2, Profile* &p3) const;

    //! Perform the congruence transformation on each bin of the profile
    template <typename T> void transform (const Jones<T>& response);

    //! Convert to the specified state
    void convert_state (Signal::State output_state);
    
  protected:
    //! The basis in which the radiation is measured
    Signal::Basis basis;

    //! The state of the polarimetric profiles
    Signal::State state;

    //! The polarimetric profiles
    Profile* data[4];

    //! Flag that the Profiles are managed by this instance
    bool mine;

    //! Efficiently forms the inplace sum and difference of two profiles
    void sum_difference (Profile* sum, Profile* difference);

    //! Set everthing to null values
    void init ();
  };

}

template <typename T>
void Pulsar::PolnProfile::transform (const Jones<T>& response)
{
  unsigned nbin = data[0]->get_nbin();
  Jones<float> response_dagger = herm(response);

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    set_Stokes (ibin, (response * get_Stokes(ibin)) * response_dagger);
}

#endif



