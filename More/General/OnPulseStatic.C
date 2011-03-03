/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/OnPulseStatic.h"
#include "Pulsar/PhaseWeight.h"

#include <iostream>
using namespace std;

Pulsar::OnPulseStatic* Pulsar::OnPulseStatic::clone () const
{
  return new OnPulseStatic (*this);
}

//! Retrieve the PhaseWeight
void Pulsar::OnPulseStatic::calculate (PhaseWeight* weight) try
{

}
catch (Error& error)
{
  throw error += "Pulsar::OnPulseStatic::calculate";
}

class Pulsar::OnPulseStatic::Interface 
  : public TextInterface::To<OnPulseStatic>
{
public:
  Interface (OnPulseStatic* instance)
  {
    if (instance)
      set_instance (instance);
#if 0
    add( &OnPulseStatic::get_threshold,
	 &OnPulseStatic::set_threshold,
	 "threshold", "threshold above which points must fall" );
#endif
  }

  std::string get_interface_name () const { return "above"; }  
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::OnPulseStatic::get_interface ()
{
  return new Interface (this);
}
