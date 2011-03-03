/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BaselineStatic.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::BaselineStatic::BaselineStatic ()
{
}

Pulsar::BaselineStatic::~BaselineStatic ()
{
}

Pulsar::BaselineStatic* Pulsar::BaselineStatic::clone () const
{
  return new BaselineStatic (*this);
}

void Pulsar::BaselineStatic::calculate (PhaseWeight* weight)
{
}

class Pulsar::BaselineStatic::Interface 
  : public TextInterface::To<BaselineStatic>
{
public:
  Interface (BaselineStatic* instance)
  {
    if (instance)
      set_instance (instance);

#if 0
    add( &BaselineStatic::get_threshold,
	 &BaselineStatic::set_threshold,
	 "threshold", "cutoff threshold used to avoid outliers" );
#endif
  }

  std::string get_interface_name () const { return "static"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::BaselineStatic::get_interface ()
{
  return new Interface (this);
}

