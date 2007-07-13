/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyAppend.h"

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"

#include "Error.h"

using namespace std;

Pulsar::FrequencyAppend::FrequencyAppend ()
{
  match.set_check_centre_frequency (false);
}

bool Pulsar::FrequencyAppend::stop (Archive* into, const Archive* from)
{
  IntegrationOrder* order_into = into->get<IntegrationOrder>();
  const IntegrationOrder* order_from = from->get<IntegrationOrder>();
  
  if (order_into || order_from)
    throw Error (InvalidState, "Pulsar::FrequencyAppend::stop",
		  "IntegrationOrder extension support not yet implemented");

  return Append::stop (into, from);
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

  for (unsigned isub=0; isub < nsubint; isub++)
    combine (into->get_Integration(isub), from->get_Integration(isub));

  into->expert()->set_nchan( into->get_nchan() + from->get_nchan() );

  double total_bandwidth = into->get_bandwidth() + from->get_bandwidth();

  double weighted_centre_frequency =
    ( into->get_bandwidth() * into->get_centre_frequency() +
      from->get_bandwidth() * from->get_centre_frequency() ) / total_bandwidth;

  into->set_bandwidth( total_bandwidth );
  into->set_centre_frequency( weighted_centre_frequency );

  if (Archive::verbose > 2)
    cerr << "Pulsar::FrequencyAppend::combine result"
      " nchan=" << into->get_nchan() << 
      " freq=" << into->get_centre_frequency() << 
      " bw=" << into->get_bandwidth() << endl;
}

void Pulsar::FrequencyAppend::combine (Integration* into, Integration* from)
{
  double weight_ratio = 1.0;

  if (into->get_duration() != 0.0)
    weight_ratio = from->get_duration() / into->get_duration();

  unsigned npol = from->get_npol();
  unsigned nchan = from->get_nchan();

  for (unsigned ipol = 0; ipol < npol; ipol++)
    for (unsigned ichan = 0; ichan < nchan; ichan++)
      from->set_weight( ichan, from->get_weight(ichan) * weight_ratio );

  into->expert()->insert(from);
}
