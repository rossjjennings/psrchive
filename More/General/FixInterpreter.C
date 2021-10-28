/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/FixInterpreter.h"
#include "Pulsar/ArchiveInterface.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Pointing.h"
#include "Pulsar/ParametersLookup.h"


#ifdef HAVE_CFITSIO
#include "Pulsar/FITSArchive.h"
#endif
#include "TextInterface.h"

using namespace std;
using namespace Pulsar;

Pulsar::FixInterpreter::FixInterpreter ()
{
  add_command 
    ( &FixInterpreter::fluxcal, 
      "fluxcal", "fix the Archive::Type (FluxCalOn or Off)" );

  add_command 
    ( &FixInterpreter::receiver, 
      "receiver", "fix the receiver information using log files");

  add_command
    ( &FixInterpreter::epoch,
      "epoch", "offset the epoch of each sub-integration",
      "usage: epoch <seconds> \n"
      "  float <seconds>  time offset in seconds \n");

  add_command
    ( &FixInterpreter::freq,
      "freq", "recompute the frequency of each channel",
      "usage: freq [sideband|offset <MHz>]\n"
      "  freq               frequency computed using standard equation \n"
      "  freq sideband      freq[i] = freq[N-1-i] and bw = -bw \n" 
      "  freq offset <MHz>  offset all frequencies by <MHz> \n");

  add_command
    ( &FixInterpreter::pointing,
      "pointing", "fix the Pointing extension info \n");

  add_command
    ( &FixInterpreter::coord,
      "coord", "fix the coordinates, based in the name \n");

#ifdef HAVE_CFITSIO
  add_command
    ( &FixInterpreter::psrfits_refmjd,
      "refmjd", "fix epoch error due to polyco REF_MJD precision in PSRFITS\n");
#endif
}

Pulsar::FixInterpreter::~FixInterpreter ()
{
}

string Pulsar::FixInterpreter::empty ()
{
  return response (Fail, help());
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::FixInterpreter::fluxcal (const string& args) try
{
  fix_flux_cal.apply (get());
  return response (Good, fix_flux_cal.get_changes());
}
catch (Error& error)
{
  return response (error);
}


// //////////////////////////////////////////////////////////////////////
//
string Pulsar::FixInterpreter::receiver (const string& args) try
{ 
  set_receiver.apply (get());
  return response (Good);
}
catch (Error& error)
{
  return response (error);
}


// //////////////////////////////////////////////////////////////////////
//
string Pulsar::FixInterpreter::epoch (const string& args) try
{
  double seconds = setup<float> (args);

  Archive* archive = get();
  unsigned nsub = archive->get_nsubint();
  
  for (unsigned isub=0; isub < nsub; isub++)
  {
    Integration* subint = archive->get_Integration(isub);
    MJD epoch = subint->get_epoch();
    epoch += seconds;
    subint->set_epoch( epoch );
  }

  return response (Good);
}
catch (Error& error)
{
  return response (error);
}

// //////////////////////////////////////////////////////////////////////
//
string Pulsar::FixInterpreter::freq (const string& args) try
{
  vector<string> arguments = setup (args);

  Archive* archive = get();

  unsigned isub,  nsub = archive->get_nsubint();
  unsigned ichan, nchan = archive->get_nchan();

  if (arguments.size() == 0)
  {
    double cfreq = archive->get_centre_frequency ();
    double bw = archive->get_bandwidth ();

    // with no arguments, apply the standard equation
    for (isub = 0; isub < nsub; isub++)
    {
      Integration* subint = archive->get_Integration (isub);

      for (ichan = 0; ichan < nchan; ichan++)
      {
	double f = cfreq - 0.5 * bw + (ichan + 0.5) * bw / nchan;
	subint->set_centre_frequency( ichan, f );
      }
    }
  }

  else if (arguments[0] == "sideband")
  {
    for (isub = 0; isub < nsub; isub++)
    {
      Integration* subint = archive->get_Integration (isub);

      vector<double> labels (nchan);
      for (ichan = 0; ichan < nchan; ichan++)
	labels[ichan] = subint->get_centre_frequency( ichan );

      for (ichan = 0; ichan < nchan; ichan++)
	subint->set_centre_frequency( ichan, labels[nchan-1-ichan] );

    }
    archive->set_bandwidth( - archive->get_bandwidth() );
  }

  else if (arguments[0] == "offset" && arguments.size() == 2)
  {
    double delta_f = fromstring<double>(arguments[1]);

    // update channel labels
    for (isub = 0; isub < nsub; isub++)
    {
      Integration* subint = archive->get_Integration (isub);

      for (ichan = 0; ichan < nchan; ichan++)
      {
	double freq = subint->get_centre_frequency( ichan );
	subint->set_centre_frequency( ichan, freq + delta_f );
      }
    }

    // update metadata
    double freq = archive->get_centre_frequency();
	archive->set_centre_frequency(freq + delta_f);
    Reference::To<TextInterface::Parser> iface = archive->get_interface();
    freq = fromstring<double>(iface->get_value("ext:obsfreq"));
    iface->set_value("ext:obsfreq",tostring<double>(freq + delta_f));
  }

  return response (Good);
}
catch (Error& error)
{
  return response (error);
}

string Pulsar::FixInterpreter::coord (const string& args) try
{
  Archive* archive = get();

  /*
    if the coordinates are already set, do nothing unless args == "clobber"
  */

  if (args != "clobber")
  {
    sky_coord current = archive->get_coordinates ();
    if (current.ra() != 0.0 || current.dec() != 0.0)
      return response (Good, "coordinates already set");
  }
  
  string name = archive->get_source ();

  Parameters::Lookup catalog;

  Reference::To<Parameters> params = catalog(name);

  string right_ascension = params->get_value ("RAJ");
  string declination = params->get_value ("DECJ");

  sky_coord coord = hmsdms (right_ascension, declination);

  archive->set_coordinates (coord);
  
  return response (Good);
}
catch (Error& error)
{
  return response (error);
}
  
string Pulsar::FixInterpreter::pointing (const string& args) try
{
  Archive* archive = get();
  unsigned nsub = archive->get_nsubint();
  for (unsigned isub=0; isub < nsub; isub++)
  {
    Integration* subint = archive->get_Integration(isub);
    Pointing* point = subint->getadd<Pointing>();
    point->set_right_ascension(archive->get_coordinates().ra());
    point->set_declination(archive->get_coordinates().dec());
    point->update(subint,archive);
  }

  return response (Good);
}
catch (Error& error)
{
  return response (error);
}

#ifdef HAVE_CFITSIO
string Pulsar::FixInterpreter::psrfits_refmjd (const string& args) try
{
  FITSArchive* archive = dynamic_cast<FITSArchive*>(get());
  if (archive)
  {
    archive->refmjd_rounding_correction();
  }
  return response (Good);
}
catch (Error &error)
{
  return response (error);
}
#endif
