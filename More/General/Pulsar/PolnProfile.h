#ifndef __PolnProfile_h
#define __PolnProfile_h

#include "Reference.h"
#include "Stokes.h"
#include "Jones.h"
#include "Types.h"

namespace Pulsar {

  class Profile;

  //! Implements polarimetric pulse profile operations.
  /*! This class does not actually store its own profiles.  Rather, it
    uses references to Reference::To<Profile> objects in order to
    manipulate the polarimetric profiles stored external to the class.
    In this sense, the PolnProfile class is currenlty used only as a
    workhorse. */
  class PolnProfile : public Reference::Able {
    
  public:
    //! Null constructor
    PolnProfile ();

    //! Construct from four externally-managed Profile objects
    PolnProfile (Signal::Basis basis, Signal::State state,
		 Reference::To<Profile>& p0, Reference::To<Profile>& p1,
		 Reference::To<Profile>& p2, Reference::To<Profile>& p3);
    
    //! Destructor
    ~PolnProfile();
    
    //! Get the Stokes 4-vector for the specified bin
    Stokes<float> get_Stokes (unsigned ibin);

    //! Set the Stokes 4-vector for the specified bin
    void set_Stokes (unsigned ibin, const Stokes<float>& stokes);

    //! Get the State of the poln profile
    Signal::State get_state () const { return state; }

    //! Perform the congruence transformation on each bin of the profile
    template <typename T> void transform (const Jones<T>& response);

    //! Convert to the specified state
    void convert_state (Signal::State output_state);
    
  protected:
    //! The basis in which the radiation is measured
    Signal::Basis basis;

    //! The state of the polarimetric profiles
    Signal::State state;

    //! References to the polarimetric profiles
    Reference::To<Profile> &p0, &p1, &p2, &p3;

    //! Efficiently forms the inplace sum and difference of two profiles
    void sum_difference (Profile* sum, Profile* difference);

    //! Set everthing to null values
    void init ();

  private:
    //! Empty reference used for null constructor
    static Reference::To<Profile> null;
  };

}

template <typename T>
void Pulsar::PolnProfile::transform (const Jones<T>& response)
{
  unsigned nbin = p0->get_nbin();
  Jones<float> response_dagger = herm(response);

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    set_Stokes (ibin, (response * get_Stokes(ibin)) * response_dagger);
}

#endif



