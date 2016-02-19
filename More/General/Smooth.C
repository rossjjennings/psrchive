/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Smooth.h"
#include "Pulsar/Profile.h"

using namespace std;

Pulsar::Smooth::Smooth ()
{
  width.set_value( Pulsar::Profile::default_duty_cycle );
}

Pulsar::Smooth::~Smooth ()
{

}

void Pulsar::Smooth::set_turns (float _turns)
{
  if (_turns < 0 || _turns >= 1.0)
    throw Error (InvalidParam, "Pulsar::Smooth::set_turns",
		 "invalid turns = %f", _turns);

  width.set_unit( Phase::Turns );
  width.set_value( _turns );
}

float Pulsar::Smooth::get_turns () const
{
  return width.get_as( Phase::Turns );
}

//! Set the number of phase bins in the bins used to smooth
void Pulsar::Smooth::set_bins (float _bins)
{
  if (_bins < 0 )
    throw Error (InvalidParam, "Pulsar::Smooth::set_bins",
		 "invalid bins = %f", _bins);

  width.set_unit( Phase::Bins );
  width.set_value( _bins );
}

//! Get the number of phase bins in the bins used to smooth
float Pulsar::Smooth::get_bins () const
{
  return width.get_as( Phase::Bins );
}

float Pulsar::Smooth::get_bins (const Profile* profile)
{
  width.set_nbin( profile->get_nbin() );
  return width.get_as( Phase::Bins );
}
 
float Pulsar::Smooth::get_turns (const Profile* profile)
{
  width.set_nbin( profile->get_nbin() );
  return width.get_as( Phase::Turns );
}
 

Pulsar::Smooth::Interface::Interface (Smooth* instance)
{
  if (instance)
    set_instance (instance);

  add( &Smooth::get_width,
       &Smooth::set_width,
       "width", "width of smoothing function" );
}

//
// the following could probably be moved it's own compilation unit
//

#include "interface_factory.h"

#include "Pulsar/SmoothMean.h"
#include "Pulsar/SmoothMedian.h"
#include "Pulsar/SmoothSinc.h"

Pulsar::Smooth* Pulsar::Smooth::factory (const std::string& name_parse)
{
  std::vector< Reference::To<Smooth> > instances;

  instances.push_back( new SmoothMean );
  instances.push_back( new SmoothMedian );
  instances.push_back( new SmoothSinc );

  return TextInterface::factory<Smooth> (instances, name_parse);
}

