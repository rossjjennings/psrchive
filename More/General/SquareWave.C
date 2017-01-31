/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SquareWave.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Smooth.h"

using namespace std;

Pulsar::SquareWave::SquareWave ()
{
  risetime = 0.03;
  threshold = 6.0;
  use_nbin = 256;
}

//! Return the signal to noise ratio
float Pulsar::SquareWave::get_snr (const Profile* profile)
{
  int hightolow, lowtohigh, buffer;
  profile->find_transitions (hightolow, lowtohigh, buffer);

  // the third argument is the variance of the mean
  double hi_mean, hi_var;
  profile->stats (&hi_mean, 0, &hi_var,
		  lowtohigh + buffer,
		  hightolow - buffer);

  double lo_mean, lo_var;
  profile->stats (&lo_mean, 0, &lo_var,
		  hightolow + buffer,
		  lowtohigh - buffer);

  if (hi_var <= 0.0 || lo_var <= 0.0)
    return 0;

  return (hi_mean - lo_mean) / sqrt(hi_var + lo_var);
}    

Pulsar::Profile* differentiate (const Pulsar::Profile* profile, unsigned off=1)
{
  Reference::To<Pulsar::Profile> difference = profile->clone();

  unsigned nbin = profile->get_nbin();
  const float* amps = profile->get_amps();
  float* damps = difference->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++)
    damps[ibin] = amps[(ibin+off)%nbin] - amps[ibin];

  return difference.release();
}

// find the transitions
void find_transitions (unsigned nbin, float* amps, vector<unsigned>& t,
		       float cutoff)
{
  for (unsigned ibin=0; ibin<nbin; ibin++) {

    bool passover = false;
    unsigned stbin = 0;

    if ( (cutoff > 0 && amps[ibin] > cutoff) || 
	 (cutoff < 0 && amps[ibin] < cutoff) ) {
      stbin = ibin;
      passover = true;
    }

    if (passover) {
      // try to avoid counting the same transition twice by searching
      // for three samples in a row with difference less than cutoff
      unsigned count = 0;
      while ( ibin < nbin ) {
	if (fabs(amps[ibin]) < fabs(cutoff))
	  count ++;
	else
	  count = 0;
	if (count > 2)
	  break;
	ibin++;
      }

      t.push_back ((stbin+ibin-3)/2);
    }
    
  }

}

void Pulsar::SquareWave::get_transitions (const Profile* profile,
					  vector<unsigned>& up,
					  vector<unsigned>& down)
{
  unsigned nbin = profile->get_nbin();

  Reference::To<Profile> clone;
  if (use_nbin && nbin/use_nbin > 1)
  {
    clone = profile->clone();
    clone->bscrunch(nbin/use_nbin);
    profile = clone;
    nbin = profile->get_nbin();
  }

  unsigned offset = (unsigned) (risetime * nbin);

  // differentiate the profile
  Reference::To<Profile> difference = differentiate (profile, offset);

  float* amps = difference->get_amps();

  // find the phase window in which the mean is closest to zero
  BaselineWindow window;
  window.set_find_mean (0.0);
  window.get_smooth()->set_turns (0.2);
  float zero = window.find_phase (nbin, amps);

  // get the noise statistics of the zero mean region
  double mean = 0;
  double variance = 0;
  difference->stats (zero, &mean, &variance);

  // cerr << "mean=" << mean << " rms=" << sqrt(variance) << endl;

  // check that the mean is actually zero
  double rms = sqrt(variance);
  if (mean > rms)
    throw Error (InvalidState, "Pulsar::SquareWave::get_transitions",
		 "mean=%lf > rms=%lf", mean, rms);

  float cutoff = threshold * rms;

  find_transitions (nbin, amps, up, cutoff);
  find_transitions (nbin, amps, down, -cutoff);
}

unsigned Pulsar::SquareWave::count_transitions (const Profile* profile)
{
  vector<unsigned> up;
  vector<unsigned> down;
  get_transitions (profile, up, down);

#if 0
  cerr << "UP=" << lowtohigh << " DOWN=" << hightolow << endl;

  cerr << "Found " << up.size() << " up & " << down.size() << " down" << endl;
  for (unsigned i=0; i<up.size(); i++)
    cerr << "up " << up[i] << endl;
  for (unsigned i=0; i<down.size(); i++)
    cerr << "down " << down[i] << endl;
#endif

  return std::min( up.size(), down.size() );
}

class Pulsar::SquareWave::Interface
  : public TextInterface::To<SquareWave>
{
public:
  Interface (SquareWave* instance)
  {
    if (instance)
      set_instance (instance);

    add( &SquareWave::get_threshold,
         &SquareWave::set_threshold,
         "threshold", "threshold used to count transitions" );

    add( &SquareWave::get_risetime,
         &SquareWave::set_risetime,
         "risetime", "turns omitted from consideration at transitions" );
  }

  std::string get_interface_name () const { return "square"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::SquareWave::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::SquareWave* Pulsar::SquareWave::clone () const
{
  return new SquareWave (*this);
}
