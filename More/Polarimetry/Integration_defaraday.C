#include "Integration.h"
#include "Profile.h"
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
  double c_sq = speed_of_light * speed_of_light;
  double cfreq = get_centre_frequency();

  // one over the centre frequency in Hz squared
  double inv_cfreq_sq = 1e-12 / (cfreq * cfreq);

  vector<Angle> phases (get_nchan());

  for (int ichan=0; ichan < get_nchan(); ichan++)
  {
    double frequency = profiles[0][ichan]->get_centre_frequency();
    double inv_freq_sq = 1e-12 / (frequency * frequency);

    phases[ichan] = 2.0 * c_sq * (rm * (inv_cfreq_sq-inv_freq_sq)
				  - rm_iono * inv_freq_sq);
  }

  cerr << "Integration::defaraday incomplete" << endl;
  //V_rotation (phases);
}
