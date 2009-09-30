/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArrivalTime.h"
#include "Pulsar/ProfileStandardShift.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/IntegrationOrder.h"

using namespace std;

Pulsar::ArrivalTime::ArrivalTime ()
{
  format = Tempo::toa::Parkes;
  skip_bad = true;
}

Pulsar::ArrivalTime::~ArrivalTime ()
{
}

//! Set the observation from which the arrival times will be derived
void Pulsar::ArrivalTime::set_observation (const Archive* archive)
{
  observation = archive;
}

//! Set the standard/template used by some phase shift estimators
void Pulsar::ArrivalTime::set_standard (const Archive* archive)
{
  standard = archive;
}

//! Set the algorithm used to estimate the phase shift
void Pulsar::ArrivalTime::set_shift_estimator (ShiftEstimator* shift)
{
  shift_estimator = shift;

  profile_shift = dynamic_cast<ProfileShiftEstimator*> (shift);
}

//! Set the format of the output time-of-arrival estimates
void Pulsar::ArrivalTime::set_format (Tempo::toa::Format fmt)
{
  format = fmt;
}

//! Add to the vector of time-of-arrival estimates
void Pulsar::ArrivalTime::get_toas (std::vector<Tempo::toa>& toas)
{
  if (observation->get<IntegrationOrder>())
    throw Error (InvalidState, "Pulsar::ArrivalTime::get_toas",
		 "Archive not in time order");

  if (standard)
  {
    string reason;
    if (!observation->standard_match (standard, reason))
      cerr << "Pulsar::ArrivalTime::get_toas WARNING " << reason << endl;

    double cf1 = observation->get_centre_frequency();
    double cf2 = standard->get_centre_frequency();
    double dfreq = fabs (cf2 - cf1);

    if (dfreq > 0.2 * cf1)
      cerr << "Pulsar::ArrivalTime::get_toas WARNING centre frequency"
	" standard= " << cf2 << " != observation=" << cf1 << endl;
  }

  std::string nsite = observation->get_telescope();
  std::string aux_txt;

  if (format == Tempo::toa::Tempo2)
    aux_txt = get_tempo2_aux_txt ();

  for (unsigned isub=0; isub<observation->get_nsubint(); isub++)
  {
    unsigned ntoa = toas.size();

    get_toas (observation->get_Integration(isub), toas);

    if (format == Tempo::toa::Parkes || format == Tempo::toa::Psrclock)
      aux_txt = observation->get_filename() + " " + tostring(isub);

    for (unsigned i=ntoa; i < toas.size(); i++)
    {
      toas[i].set_subint (isub);
      toas[i].set_telescope (nsite);

      string txt = toas[i].get_auxilliary_text ();
      if (txt.length())
	txt = aux_txt + " " + txt;
      else
	txt = aux_txt;

      toas[i].set_auxilliary_text (txt);
    }
  }
}

void Pulsar::ArrivalTime::get_toas (const Pulsar::Integration* subint,
				    std::vector<Tempo::toa>& toas)
{
  // get the topocentric folding period
  double folding_period = subint->get_folding_period();

  // get the mid time of the integration (rise time of bin 0 in each profile)
  MJD epoch = subint->get_epoch ();

  ProfileStandardShift* shift = 0;

  if (standard && standard->get_nchan() > 1)
    shift = dynamic_cast<ProfileStandardShift*>( shift_estimator.get() );

  for (unsigned ichan=0; ichan < subint->get_nchan(); ++ichan)
  {
    if (shift)
      shift->set_standard (standard->get_Profile (0,0,ichan));

    const Profile* profile = subint->get_Profile (0, ichan);

    if (skip_bad && profile->get_weight() == 0)
      continue;
    
    try {
      
      Tempo::toa toa = get_toa (profile, epoch, folding_period);
      
      if (subint->get_dedispersed())
	toa.set_frequency (subint->get_centre_frequency());
      else
	toa.set_frequency (profile->get_centre_frequency());

      toa.set_channel (ichan);

      // would like to see this go somewhere else
      if (format == Tempo::toa::Parkes || format == Tempo::toa::Psrclock)
	toa.set_auxilliary_text( tostring(ichan) );      

      toas.push_back (toa);
    }
    catch (Error& error)
    {
      if (Archive::verbose > 3)
	cerr << "Pulsar::Integration::toas error" << error << endl;
      continue;
    }
  }
}

Tempo::toa Pulsar::ArrivalTime::get_toa (const Pulsar::Profile* profile,
					 const MJD& mjd, double period)
{
  Tempo::toa retval (format);

  if (!profile_shift)
    throw Error (InvalidState, "Pulsar::ArrivalTime::get_toa",
		 "shift estimator is not a profile shift estimator");

  profile_shift->set_observation (profile);
  
  Estimate<double> Ephase = profile_shift->get_shift ();

  retval.set_phase_shift (Ephase.get_value());
  retval.set_arrival     (mjd + Ephase.get_value() * period);
  retval.set_error       (Ephase.get_error() * period * 1e6);

  if (Archive::verbose > 3)
  {
    fprintf (stderr, "Pulsar::ArrivalTime::get_toa created:\n");
    retval.unload (stderr);
  }

  return retval;
}










void Pulsar::ArrivalTime::set_format (const string& fmt)
{
  if (strcasecmp(fmt.c_str(),"parkes")==0)
    format = Tempo::toa::Parkes;

  else if (strcasecmp(fmt.c_str(),"princeton")==0)
    format = Tempo::toa::Princeton;

  else if (strcasecmp(fmt.c_str(),"itoa")==0)
    format = Tempo::toa::ITOA;

  else if (strcasecmp(fmt.c_str(),"psrclock")==0)
    format = Tempo::toa::Psrclock;

  else if (strcasecmp(fmt.c_str(),"tempo2")==0)
    format = Tempo::toa::Tempo2;
}


