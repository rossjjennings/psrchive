/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StaticBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::StaticBaseline::StaticBaseline ()
{
}

Pulsar::StaticBaseline::~StaticBaseline ()
{
}

Pulsar::StaticBaseline* Pulsar::StaticBaseline::clone () const
{
  return new StaticBaseline (*this);
}

void Pulsar::StaticBaseline::calculate (PhaseWeight* weight)
{
}

class Pulsar::StaticBaseline::Interface 
  : public TextInterface::To<StaticBaseline>
{
public:
  Interface (StaticBaseline* instance)
  {
    if (instance)
      set_instance (instance);

#if 0
    add( &StaticBaseline::get_threshold,
	 &StaticBaseline::set_threshold,
	 "threshold", "cutoff threshold used to avoid outliers" );
#endif
  }

  std::string get_interface_name () const { return "static"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::StaticBaseline::get_interface ()
{
  return new Interface (this);
}

