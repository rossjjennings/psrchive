#include "Profile.h"
#include "Error.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::average
//
/*!
  It can be easily shown that a series of additions using this operator
  preserves the simple relationship:

  \f$ \bar{x} = \sum_{i=1}^N W(x_i) x_i / W(\bar{x}) \f$

  where \f$ W(x_i) \f$ is the weight assigned to \f$ x_i \f$ and

  \f$ W(\bar{x}) = \sum_{i=1}^N W(x_i) \f$
*/
const Pulsar::Profile& Pulsar::Profile::average (const Profile& profile, 
						 double sign)
{
  if (nbin != profile.get_nbin())
    throw Error (InvalidRange, "Pulsar::Profile::average",
		 "nbin=%d != profile.nbin=%d", nbin, profile.get_nbin());

  try {

    // check if the addition will result in some undefined state
    if (state != profile.get_state())
      state = Poln::None;
    
    float* amps1 = amps;
    const float* amps2 = profile.get_amps();
    
    double weight1 = weight;
    double weight2 = profile.get_weight();
    
    weight = weight1 + weight2;
    
    double norm = 0.0;
    if (weight != 0)
      norm = 1.0 / weight;
    
    for (int ibin=0; ibin<nbin; ibin++) {
      *amps1 = norm * ( double(*amps1)*weight1 + sign*double(*amps2)*weight2 );
      amps1 ++; amps2 ++;
    }

  }
  catch (Error& error) {
    throw error += "Profile::average";
  }

  return *this;
}
