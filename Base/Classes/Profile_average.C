#include "Pulsar/Profile.h"
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
      state = Signal::None;
    
    float* amps1 = amps;
    const float* amps2 = profile.get_amps();
    
    double weight1 = weight;
    double weight2 = profile.get_weight();
    
    weight = weight1 + weight2;
    
    double norm = 0.0;
    if (weight != 0)
      norm = 1.0 / weight;
    
    for (unsigned ibin=0; ibin<nbin; ibin++) {
      *amps1 = norm * ( double(*amps1)*weight1 + sign*double(*amps2)*weight2 );
      amps1 ++; amps2 ++;
    }

  }
  catch (Error& error) {
    throw error += "Profile::average";
  }

  return *this;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::morphological_difference
//
/*!
  This function is designed to be used for detecting morphological
  differences in pulse shape irrespective of the amplitudes of the
  individual profiles involved.
*/

Pulsar::Profile* Pulsar::Profile::morphological_difference (const Profile& profile,
							    bool scale_by_bins)
{
  if (get_nbin() != profile.get_nbin())
    throw Error (InvalidRange, "Pulsar::Profile::morphological_difference",
		 "incompatible number of phase bins");

  Pulsar::Profile* temp1 = new Pulsar::Profile(*this);
  Pulsar::Profile* temp2 = new Pulsar::Profile(profile);

  float ephase, snrfft, esnrfft; 
  double phase = temp1->shift (*temp2, ephase, snrfft, esnrfft);

  temp1->rotate(phase);

  float minphs = 0.0;

  minphs = temp1->find_min_phase(0.6);
  *temp1 -= (temp1->mean(minphs));

  minphs = temp2->find_min_phase(0.6);
  *temp2 -= (temp2->mean(minphs));

  if (!scale_by_bins) { 
    float ratio = 100.0 / temp1->sum();
    *temp1 *= ratio;
    ratio = 100.0 / temp2->sum();
    *temp2 *= ratio;
    
    float* amps1 = temp1->get_amps();
    float* amps2 = temp2->get_amps();
    
    for (unsigned i = 0; i < temp1->get_nbin(); i++) {
      amps1[i] = amps1[i] - amps2[i];
    }
  }
  else {
    float ratio = temp1->sum() / temp2->sum();
    *temp2 *= ratio;
  
    float* amps1 = temp1->get_amps();
    float* amps2 = temp2->get_amps();
    float temp = 0.0;
    
    int rise = 0;
    int fall = 0;

    temp1->find_peak_edges(rise, fall);
    
    for (int i = 0; i < int(temp1->get_nbin()); i++) {
      temp = amps1[i];
      amps1[i] = amps1[i] - amps2[i];
      if ( i > rise && i < fall )
	amps1[i] /= temp;
      else
	amps1[i] = 0.0;
    }
  }

  return temp1;
}




