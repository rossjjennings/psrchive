/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Weight.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"

Pulsar::Weight::Weight ()
{
  fscrunch_factor.disable_scrunch();
} 

void Pulsar::Weight::operator () (Archive* archive)
{
  if (archive->get_state() == Signal::Coherence)
    archive->convert_state (Signal::Stokes);

  if (!fscrunch_factor.scrunch_enabled ())
    for (unsigned isub = 0; isub < archive->get_nsubint(); isub++)
      weight (archive->get_Integration (isub));
  else
  {
    for (unsigned isub = 0; isub < archive->get_nsubint(); isub++)
    {
      Reference::To<Integration> data = archive->get_Integration (isub);
      Reference::To<Integration> copy = data->clone();

      Pulsar::fscrunch (copy->expert(), fscrunch_factor);
      weight (copy);
      copy_weights (copy, data);
    }
  }
}

void Pulsar::Weight::copy_weights (const Integration* from,
				   Integration* to)
{
  unsigned from_nchan = from->get_nchan();
  unsigned nscrunch = to->get_nchan() / from_nchan;
  
  for (unsigned ichan=0; ichan<from_nchan; ichan++) 
  {
    float wt = from->get_weight (ichan);
    
    for (unsigned jchan=0; jchan < nscrunch; jchan++)
    {
      unsigned ch = ichan*nscrunch + jchan;
      if (to->get_weight (ch) != 0)
	to->set_weight (ch, wt);
    }
  }
}

// Text interface to the Weight class
class Pulsar::Weight::Interface : public TextInterface::To<Weight>
{
 public:
  Interface (Weight* = 0);
};

// Text interface to Weight
TextInterface::Parser* Pulsar::Weight::get_interface ()
{
  return new Interface (this);
}

Pulsar::Weight::Interface::Interface (Weight* instance)
{
  if (instance)
    set_instance (instance);

  add( &Weight::get_fscrunch,
       &Weight::set_fscrunch,
       "fscrunch", "Compute weights using fscrunched clone" );
}
