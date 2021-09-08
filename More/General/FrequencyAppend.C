/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyAppend.h"
#include "Pulsar/WeightInterpreter.h"
#include "Pulsar/Dispersion.h"
#include "Pulsar/FaradayRotation.h"

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/ArchiveMatch.h"

#include "Pulsar/ArchiveExtension.h"
#include "Pulsar/FITSHdrExtension.h"

#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include <Pulsar/DigitiserCounts.h>

#include "Pulsar/Generator.h"
#include "Pulsar/Parameters.h"
#include "Pulsar/Predictor.h"

#include "Error.h"

using namespace Pulsar;
using namespace std;

void ensure_FrequencyAppend_linkage()
{
}

Pulsar::Option<FrequencyAppend::Weight> FrequencyAppend::weight_strategy
(
 new Pulsar::WeightInterpreter (FrequencyAppend::weight_strategy),

 "FrequencyAppend::weight", "radiometer",

 "Weighting policy",

 "The name of the algorithm used to weight each profile.\n"
 "Possible values: radiometer, time, snr, none"
);

Pulsar::Option<bool> FrequencyAppend::force_new_predictor
(
 "FrequencyAppend::force_new_predictor", false,

 "Force creation of a new predictor while frequency appending",

 "If true, frequency append will always create a new predictor.\n"
 "See bug report #400 for rationale behind this option. In brief, \n"
 "it helps to fix a problem with predictor not spanning required \n"
 "epochs in some cases.\n" 
 "Setting this to true will adversly affect psrchive's performance.\n"
);

const Pulsar::Archive::Match*
Pulsar::FrequencyAppend::get_mixable_policy (const Archive* a)
{
  Reference::To<Archive::Match> mixable = a->get_mixable()->clone();
  mixable->set_check_centre_frequency (false);
  mixable->set_check_bandwidth (false);
  mixable->set_check_nchan (false);

  return mixable.release();
}

bool Pulsar::FrequencyAppend::stop (Archive* into, const Archive* from)
{
  IntegrationOrder* order_into = into->get<IntegrationOrder>();
  const IntegrationOrder* order_from = from->get<IntegrationOrder>();
  
  if (order_into || order_from)
    throw Error (InvalidState, "Pulsar::FrequencyAppend::stop",
		  "IntegrationOrder extension support not yet implemented");

  // even if there are no sub-integrations, frequency append the extensions
  return false;
}

void Pulsar::FrequencyAppend::check (Archive* into, const Archive* from)
{
  Append::check (into, from);

  if (into->get_nsubint() != from->get_nsubint()) 
    throw Error (InvalidState, "Pulsar::FrequencyAppend::check",
		 "Archives have different numbers of sub-integrations;"
		 " into=%u from=%u", into->get_nsubint(), from->get_nsubint());
}

void Pulsar::FrequencyAppend::combine (Archive* into, Archive* from)
{
  unsigned nsubint = into->get_nsubint();

  check_phase = true;

  if (force_new_predictor)
    generate_new_predictor( into, from );

  if (from->get_dedispersed() && from->has_model())
  {
    double into_frequency = into->get_model()->get_observing_frequency ();
    double from_frequency = from->get_model()->get_observing_frequency ();
    double centre_frequency = from->get_centre_frequency ();

    if (from_frequency == centre_frequency)
    {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FrequencyAppend::combine "
          "dedispersed and phase at infinite frequency" << endl;

      equal_models = true;
      check_phase = false;
      insert_model = false;
    }
    else if (from_frequency == into_frequency)
    {
      if (Archive::verbose > 2)
        cerr << "Pulsar::FrequencyAppend::combine "
          "dedispersed and matching predictor frequencies" << endl;

      insert_model = false;
    }
    else
      throw Error (InvalidState, "Pulsar::FrequencyAppend::combine",
		   "predictor frequency mismatch: from=%lf != into=%lf \n\t"
	           "from centre frequency=%lf",
		   from_frequency, into_frequency, centre_frequency);
  }
  else
    check_phase = true;

  for (unsigned isub=0; isub < nsubint; isub++)
    combine (into->get_Integration(isub), from->get_Integration(isub));

  into->expert()->set_nchan( into->get_nchan() + from->get_nchan() );

  double total_bandwidth = into->get_bandwidth() + from->get_bandwidth();

  double weighted_centre_frequency =
    ( into->get_bandwidth() * into->get_centre_frequency() +
      from->get_bandwidth() * from->get_centre_frequency() ) / total_bandwidth;

  into->set_bandwidth( total_bandwidth );
  into->set_centre_frequency( weighted_centre_frequency );

  unsigned next = into->get_nextension();
  for (unsigned iext=0; iext < next; iext++)
    into->get_extension(iext)->frequency_append (into, from);
  
  FITSHdrExtension *fitshdrext = into->get<FITSHdrExtension>();
  if (fitshdrext != NULL)
  {
    fitshdrext->set_obsfreq(weighted_centre_frequency);
    fitshdrext->set_obsbw(total_bandwidth);
    fitshdrext->set_obsnchan(into->get_nchan() + from->get_nchan());
  }

  if (Archive::verbose > 2)
    cerr << "Pulsar::FrequencyAppend::combine result"
      " nchan=" << into->get_nchan() << 
      " freq=" << into->get_centre_frequency() << 
      " bw=" << into->get_bandwidth() << endl;
}

//! Initialize an archive for appending
void Pulsar::FrequencyAppend::init (Archive* into)
{
  if (!weight_strategy.get_value())
    return;

  const unsigned nsubint = into->get_nsubint();

  for (unsigned isub=0; isub < nsubint; isub++)
    weight_strategy.get_value() ( into->get_Integration (isub) );
}

void Pulsar::FrequencyAppend::combine (Integration* into, Integration* from)
try 
{
  if (weight_strategy.get_value())
    weight_strategy.get_value() ( from );

  into->expert()->insert(from);

  if (check_phase && into->get_dedispersed())
  {
    Dispersion xform;
    xform.match (into, from);
  }

  if (into->get_faraday_corrected())
  {
    FaradayRotation xform;
    xform.match (into, from);
  }
}
catch (Error& error)
{
  throw error += "Pulsar::FrequencyAppend::combine";
}

//! Generate a new predictor spanning the time and frequency range of both into and from
void Pulsar::FrequencyAppend::generate_new_predictor( Archive* into, Archive* from )
try
{
  // this is almost an exact copy of the Archive::update_model, maybe merge the two pieces of code?i
  // In fact I wasn't sure why simply calling update_model( MJD&, ... ) didn't suffice here so perhaps this is an overkill.
  if ( !into->has_ephemeris() )
    throw Error (InvalidState, "Pulsar::FrequencyAppend::generate_new_predictor",
		 "no Pulsar::Parameters available");
  // Get a generator:
  if (Archive::verbose > 2)
    cerr << "Pulsar::FrequencyAppend::generate_new_predictor Predictor::policy="
         << Predictor::get_policy() << endl;
  Reference::To<Generator> generator;
  if (into->has_model() && Pulsar::Predictor::get_policy() == Pulsar::Predictor::Input)
  {
    if (Archive::verbose > 2)
      cerr << "Pulsar::FrequencyAppend::generate_new_predictor get matching generator "
              "from current predictor" << endl;
    generator = into->get_model()->generator();
  }
  else if (Pulsar::Predictor::get_policy() == Pulsar::Predictor::Default)
  {
    if (Archive::verbose > 2)
      cerr << "Pulsar::FrequencyAppend::generate_new_predictor using default generator" << endl;
    generator = Generator::get_default();
  }
  else
  {
    if (Archive::verbose > 2)
      cerr << "Pulsar::FrequencyAppend::generate_new_predictor produce generator"
	" that matches parameters" << endl;
    generator = Generator::factory ( into->get_ephemeris() );
  }

  generator->set_parameters( into->get_ephemeris());
  //Reference::To<Generator> generator = into->get_model()->generator();
  double into_bandwidth = into->get_bandwidth ();
  double from_bandwidth = from->get_bandwidth ();
  double into_freq = into->get_centre_frequency ();
  double from_freq = from->get_centre_frequency ();
  double bottom_freq, top_freq;
  //psradd sorts filenames so can't rely on input being in any reasonable order
  // First determine the complete frequency range
  if ( from_freq < into_freq )
  {
    bottom_freq = from_freq - 0.5 * from_bandwidth;
    top_freq = into_freq + 0.5 * into_bandwidth;
  } else {
    bottom_freq = into_freq - 0.5 * into_bandwidth;
    top_freq = from_freq + 0.5 * from_bandwidth;
  }
  // Now determine the MJD range
  MJD earliest_MJD, latest_MJD;
  MJD into_start = into->start_time() + (into->end_time() - into->start_time()) / 2. - into->integration_length() / 2.;
  MJD from_start = from->start_time() + (from->end_time() - from->start_time()) / 2. - from->integration_length() / 2.;
  if ( into_start < from_start )
  {
    earliest_MJD = into_start;
    latest_MJD = from_start + from->integration_length();
  } else {
    earliest_MJD = from_start;
    latest_MJD = into_start + into->integration_length();
  }

  generator->set_frequency_span( bottom_freq, top_freq );
  generator->set_site( into->get_telescope() );
  generator->set_time_span( earliest_MJD, latest_MJD );
  // inject the new model
  into->set_model( generator->generate() );
}
catch (Error& error)
{
  throw error += "Pulsar::FrequencyAppend::generate_new_predictor";
}
