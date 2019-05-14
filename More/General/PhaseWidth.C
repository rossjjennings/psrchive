/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWidth.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include <math.h>

using namespace std;

Pulsar::PhaseWidth::PhaseWidth ()
{
  error = 0;
  baseline_duty_cycle = 0.15;
  fraction_of_maximum = 0.5;
}

// defined in width.C
extern float width (const Pulsar::Profile* profile,
		    float& error, float pc, float dc);

double Pulsar::PhaseWidth::get_width_turns (const Profile* profile)
{
  baseline_duty_cycle.set_nbin( profile->get_nbin() );
  error.set_nbin( profile->get_nbin() );

  float error_tmp;
  return width (profile, error_tmp, fraction_of_maximum*100,
		baseline_duty_cycle.get_as(Phase::Turns));

  error.set_value( error_tmp );
}

void Pulsar::PhaseWidth::set_fraction_of_maximum (float x)
{
  if (!(x>0 && x<1))
    throw Error (InvalidParam, "Pulsar::PhaseWidth::set_fraction_of_maximum",
		 "fraction=%f must be >0 and <1", x);
  fraction_of_maximum = x;
}


class Pulsar::PhaseWidth::Interface
  : public TextInterface::To<PhaseWidth>
{
public:
  Interface (PhaseWidth* instance)
  {
    if (instance)
      set_instance (instance);

    add( &PhaseWidth::get_baseline_duty_cycle,
	 &PhaseWidth::set_baseline_duty_cycle,
	 "bdc", "Baseline duty cycle" );

    add( &PhaseWidth::get_fraction_of_maximum,
	 &PhaseWidth::set_fraction_of_maximum,
	 "fmax", "Fraction of maximum" );

    add( &PhaseWidth::get_error,
	 "error", "Error in last width estimate" );
  }

  std::string get_interface_name () const { return "transitions"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::PhaseWidth::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::PhaseWidth* Pulsar::PhaseWidth::clone () const
{
  return new PhaseWidth (*this);
}
