#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Angle.h"
#include "Physical.h"

/*!
  \param rm rotation measure
  \param rm_iono rotation measure due to ionosphere

  \pre baseline has been removed
  \pre archive has full polarimetric information
*/
void Pulsar::Integration::defaraday (double rm, double rm_iono)
{
  if (get_state() != Signal::Stokes)
    throw Error(InvalidState, "Integration::defaraday", "State != Signal::Stokes");
  
  double c_sq = speed_of_light * speed_of_light;
  double cfreq = get_centre_frequency();
  
  // one over the centre frequency in Hz squared
  double inv_cfreq_sq = 1e-12 / (cfreq * cfreq);
  
  Angle phase;
  
  for (unsigned ichan=0; ichan < get_nchan(); ichan++) {
    
    double frequency = get_frequency(ichan);
    double inv_freq_sq = 1e-12 / (frequency * frequency);
    
    phase = 2.0 * c_sq * (rm * (inv_cfreq_sq-inv_freq_sq)
			  - rm_iono * inv_freq_sq);
    
    float sp = sin(phase.getradians());
    float cp = cos(phase.getradians());
    
    if(verbose) cerr << "Integration::defaraday rotating: " 
		     << ichan << "\t" << cp << "  " << sp << endl;
    
    Pulsar::Profile Utemp;
    Pulsar::Profile Vtemp;
    
    Utemp = *(profiles[2][ichan]);
    Vtemp = *(profiles[3][ichan]);

    Utemp *= cp;
    Vtemp *= sp;
    
    Utemp -= Vtemp;
    
    Reference::To<Pulsar::Profile> Unew = Utemp.clone();
    
    Utemp = *(profiles[2][ichan]);
    Vtemp = *(profiles[3][ichan]);
    
    Vtemp *= cp;
    Utemp *= sp;
    
    Vtemp += Utemp;
    
    Reference::To<Pulsar::Profile> Vnew = Vtemp.clone();
    
    profiles[2][ichan] = Unew;
    profiles[3][ichan] = Vnew;
  }
}

