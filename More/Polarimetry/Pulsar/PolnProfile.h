#ifndef __Pulsar_PolnProfile_h
#define __Pulsar_PolnProfile_h

#include "Reference.h"
#include "Stokes.h"
#include "Jones.h"
#include "Types.h"

#ifdef sun
#include <ieeefp.h>
#endif

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

    //! Flag set when PolnProfile::transform should correct Profile::weight
    static bool correct_weights;

    //! Null constructor
    PolnProfile ();

    //! Construct from four externally-managed Profile objects
    PolnProfile (Signal::Basis basis, Signal::State state,
		 Profile* p0, Profile* p1, Profile* p2, Profile* p3);
    
    //! Destructor
    ~PolnProfile();
    
    //! Get the number of bins
    unsigned get_nbin () const;

    //! Returns a pointer to the start of the array of amplitudes
    const float* get_amps (unsigned ipol) const;

    //! Get the State of the poln profile
    Signal::State get_state () const { return state; }

    //! Get the Stokes 4-vector for the specified bin
    Stokes<float> get_Stokes (unsigned ibin);

    //! Set the Stokes 4-vector for the specified bin
    void set_Stokes (unsigned ibin, const Stokes<float>& stokes);

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
    Reference::To<Profile> profile[4];

    //! Efficiently forms the inplace sum and difference of two profiles
    void sum_difference (Profile* sum, Profile* difference);

    //! Set everthing to null values
    void init ();

  };

}

template <typename T>
void Pulsar::PolnProfile::transform (const Jones<T>& response)
{
  unsigned nbin = profile[0]->get_nbin();

  float Gain = abs( det(response) );
  if (!finite(Gain))
    throw Error (InvalidParam, "Pulsar::PolnProfile::transform",
                 "non-invertbile response.  det(J)=%f", Gain);

  Jones<float> response_dagger = herm(response);

  for (unsigned ibin = 0; ibin < nbin; ibin++)
    set_Stokes (ibin, (response * get_Stokes(ibin)) * response_dagger);

  if (correct_weights)
    for (unsigned ipol=0; ipol < 4; ipol++)
      profile[ipol]->set_weight ( profile[ipol]->get_weight() / Gain );

}

#endif



