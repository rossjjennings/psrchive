/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SquareWave.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/BaselineWindow.h"
#include "Pulsar/GaussianBaseline.h"

#include "Pulsar/Smooth.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/CalInfoExtension.h"

#include <fstream>

using namespace std;

Pulsar::Option<float>
Pulsar::SquareWave::transition_phase
(
 "SquareWave::transition_phase", -1.0,

 "Pulse phase of cal transition [turns]",

 "If this option is set to a non-negative value, calibration signals\n"
 "will be assumed to transition from the low to high state at the specified\n"
 "phase, and back to low 0.5 turns later.  Otherwise, the default behavior\n"
 "is for psrchive to attempt to automatically determine the transition phase."
);

Pulsar::SquareWave::SquareWave ()
{
  risetime = 0.03;
  threshold = 6.0;
  use_nbin = 256;
  verbose = false;
  outlier_threshold = 3.0;
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

/*!  
  This method identifies significant transitions between high and
  low states using the differences in fluxes between phase bins
  separated by risetime turns.  Significant transitions occur where
  the absolute value of the difference in flux exceeds a threshold
  times the standard deviation of the baseline noise.
*/
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


#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Estimate.h"
#include "Error.h"

void init (Pulsar::PhaseWeight& array, int start, int end)
{
  int nbin = array.get_nbin();
  
  if (end < start)
    end += nbin;

  array.set_all (0);
  
  for (int i=start; i<end; i++)
    array[i%nbin] = 1.0;
}

Pulsar::PhaseWeight* Pulsar::SquareWave::get_mask (const Pulsar::Profile* prof,
						   bool on, int start, int end)
{
  GaussianBaseline baseline;
  baseline.set_threshold (outlier_threshold);
  baseline.set_run_postprocessing (false);
  
  PhaseWeight mask (prof->get_nbin());
  init (mask, start, end);
  baseline.set_include (&mask);
  
  BaselineWindow first;
  first.set_find_maximum (on);
  baseline.set_initial_baseline (&first);

  return baseline.operate (prof);
}

void Pulsar::SquareWave::levels (const Pulsar::Integration* subint,
				 vector<vector<Estimate<double> > >& high,
				 vector<vector<Estimate<double> > >& low)
{
  unsigned nstate = 2;      // Default to normal 2-state square wave

  if (subint->expert()->has_parent())
  {  
    // Get CalInfo extension to see what cal type is
    Reference::To<const CalInfoExtension> ext;

    const Archive* parent = subint->expert()->get_parent();
    ext = parent->get<CalInfoExtension>();

    if (ext)
    {
      if (verbose)
	cerr << "Pulsar::SquareWave::levels CalInfoExtension::cal_nstate="
	     << ext->cal_nstate << endl;
      nstate = ext->cal_nstate;
    }
  }

  if (nstate<2 || nstate>3) 
    throw Error (InvalidState, "Pulsar::SquareWave::levels", 
        "unexpected nstate = %d", nstate);

  unsigned nbin = subint->get_nbin ();
  if (nbin==0)
    throw Error (InvalidState, "Pulsar::SquareWave::levels", "nbin = 0");

  unsigned npol = subint->get_npol ();
  if (npol==0)
    throw Error (InvalidState, "Pulsar::SquareWave::levels", "npol = 0");

  unsigned nchan = subint->get_nchan ();
  if (nchan==0)
    throw Error (InvalidState, "Pulsar::SquareWave::levels", "nchan = 0");

  Reference::To<Integration> copy = subint->total ();
  Reference::To<Profile> total = copy->get_Profile(0,0);

  if (verbose)
    cerr << "Pulsar::SquareWave::levels"
      " call Profile::find_transitions" << endl;

  int hightolow, lowtohigh, buffer;
  total->find_transitions (hightolow, lowtohigh, buffer);

  // Profile::find_transitions returns values in bins.  If transition_phase
  // is set, we will override that here.
  if (transition_phase>=0.0)
  {
    lowtohigh = int(transition_phase * nbin) % nbin;
    hightolow = (lowtohigh + nbin/2) % nbin;
  }
  
  high.resize (npol);
  low.resize (npol);
  
  // Standard bin ranges for 2-state cal pulse
  int high_start = lowtohigh + buffer;
  int high_end = hightolow - buffer;
  int low_start = hightolow + buffer;
  int low_end = lowtohigh - buffer;

  // For a "3-state" cal (eg Nancay), we want to ignore the 2nd half
  // of the high state.  This could be generalized/improved to do
  // something more intelligent at some point.
  if (nstate==3)
  { 
    int high_mid = (hightolow > lowtohigh) 
      ? (lowtohigh + hightolow) / 2
      : (lowtohigh + nbin + hightolow) / 2;
    if (high_mid > nbin) 
      high_mid -= nbin;
    high_end = high_mid - buffer;
    if (verbose) 
      cerr << "Pulsar::SquareWave::levels using 3-state cal" << endl;
  }

  Reference::To<PhaseWeight> high_mask;
  Reference::To<PhaseWeight> low_mask;
  double variance_correction = 1.0;
  
  if (verbose)
    cerr << "SquareWave::levels threshold=" << outlier_threshold << endl;
  
  if (outlier_threshold)
  {
    high_mask = get_mask (total, true, high_start, high_end);
    low_mask = get_mask (total, false, low_start, low_end);

    variance_correction
      = GaussianBaseline::get_variance_correction (outlier_threshold);
    
#if _DEBUG  
    ofstream out ("squarewave.txt");
    for (unsigned i=0; i < nbin; i++)
      out << i
	  << " " << high_mask->get_weights()[i]
	  << " " << low_mask->get_weights()[i]
	  << endl;
#endif
  }	
  
  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    high[ipol].resize(nchan);
    low[ipol].resize(nchan);

    for (unsigned ichan=0; ichan<nchan; ichan++) {

      high[ipol][ichan] = low[ipol][ichan] = 0.0;

      if (subint->get_weight(ichan) == 0)
	continue;

      const Profile* profile = subint->get_Profile (ipol, ichan);

      if (outlier_threshold)
      {
	high_mask->stats (profile, &(high[ipol][ichan].val), 0,
			  &(high[ipol][ichan].var));
	
	low_mask->stats (profile, &(low[ipol][ichan].val), 0,
			 &(low[ipol][ichan].var));

	// after clipping at some threshold, the variance is underestimated
	high[ipol][ichan].var *= variance_correction;
	low[ipol][ichan].var *= variance_correction;
      }
      else
      {
	profile->stats (&(high[ipol][ichan].val), 0,
			&(high[ipol][ichan].var),
			high_start, high_end);
	
	profile->stats (&(low[ipol][ichan].val), 0,
			&(low[ipol][ichan].var),
			low_start, low_end);
      }
      
      // for linear X and Y: if on cal is lower than off cal, flag bad data
      if (npol <= 2
	  && (high[ipol][ichan].val < 0 || low[ipol][ichan].val < 0
	      || high[ipol][ichan].val < low[ipol][ichan].val))
      {
	if (verbose) cerr << "Pulsar::SquareWave::levels"
		       " - bad levels for channel " << ichan 
			  << " poln " << ipol 
			  << " mean high " << high[ipol][ichan].val
			  << " mean low " << low[ipol][ichan].val << endl;
	high[ipol][ichan] = low[ipol][ichan] = 0.0;
      }
    }
  }
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
