#ifndef __Pulsar_PolnProfile_h
#define __Pulsar_PolnProfile_h

#include "Pulsar/Profile.h"
#include "Stokes.h"
#include "Jones.h"

namespace Pulsar {

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
    virtual ~PolnProfile();

    //! Clone operator
    virtual PolnProfile* clone () const;

    //! Set the number of bins
    void resize (unsigned nbin);

    //! Get the number of bins
    unsigned get_nbin () const;

    //! Get the specifed profile
    const Profile* get_Profile (unsigned ipol) const;

    //! Returns a const pointer to the start of the array of amplitudes
    const float* get_amps (unsigned ipol) const;

    //! Returns a pointer to the start of the array of amplitudes
    float* get_amps (unsigned ipol);

    //! Get the Basis of the poln profile
    Signal::Basis get_basis () const { return basis; }

    //! Get the State of the poln profile
    Signal::State get_state () const { return state; }

    //! Get the Stokes 4-vector for the specified bin
    Stokes<float> get_Stokes (unsigned ibin);

    //! Set the Stokes 4-vector for the specified bin
    void set_Stokes (unsigned ibin, const Stokes<float>& stokes);

    //! Get the coherency matrix for the specified bin
    Jones<double> get_coherence (unsigned ibin);

    //! Set the coherency matrix for the specified bin
    void set_coherence (unsigned ibin, const Jones<double>& coherency);

    //! Returns the sum of all amplitudes
    double sum (int bin_start=0, int bin_end=0) const;

    //! Returns the sum of all amplitudes squared
    double sumsq (int bin_start=0, int bin_end=0) const;

    //! Perform the congruence transformation on each bin of the profile
    void transform (const Jones<double>& response);

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

    //! Get the specifed profile (non-const version)
    Profile* get_profile (unsigned ipol);

  };

}


#endif



