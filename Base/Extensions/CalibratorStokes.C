/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CalibratorStokes.h"

//! Default constructor
Pulsar::CalibratorStokes::CalibratorStokes ()
  : Extension ("CalibratorStokes")
{

}

//! Copy constructor
Pulsar::CalibratorStokes::CalibratorStokes (const CalibratorStokes& extension)
  : Extension ("CalibratorStokes")
{
  operator = (extension);
}

//! Assignment operator
const Pulsar::CalibratorStokes&
Pulsar::CalibratorStokes::operator= (const CalibratorStokes& extension)
{
  stokes = extension.stokes;

  return *this;
}

//! Destructor
Pulsar::CalibratorStokes::~CalibratorStokes ()
{
}

//! Set the number of frequency channels
void Pulsar::CalibratorStokes::set_nchan (unsigned nchan)
{
  stokes.resize (nchan);
}

//! Get the number of frequency channels
unsigned Pulsar::CalibratorStokes::get_nchan () const
{
  return stokes.size();
}

//! Set the validity flag for the specified channel
void Pulsar::CalibratorStokes::set_valid (unsigned ichan, bool valid)
{
  range_check (ichan, "Pulsar::CalibratorStokes::set_valid");

  float validity = (valid) ? 1.0 : 0.0;

  stokes[ichan][0].val = validity;
}

//! Get the validity flag for the specified channel
bool Pulsar::CalibratorStokes::get_valid (unsigned ichan) const
{
  range_check (ichan, "Pulsar::CalibratorStokes::get_valid");

  return stokes[ichan][0].val != 0.0;
}

//! Set the Stokes parameters of the specified frequency channel
void Pulsar::CalibratorStokes::set_stokes (unsigned ichan,
					   const Stokes< Estimate<float> >& s)
{
  range_check (ichan, "Pulsar::CalibratorStokes::set_stokes");

  stokes[ichan] = s;
  stokes[ichan][0] = 1.0;
}

//! Get the Stokes parameters of the specified frequency channel
Stokes< Estimate<float> > 
Pulsar::CalibratorStokes::get_stokes (unsigned ichan) const
{
  range_check (ichan, "Pulsar::CalibratorStokes::get_stokes");

  return stokes[ichan];
}

void Pulsar::CalibratorStokes::range_check (unsigned ichan, 
					    const char* method) const
{
  if (ichan >= stokes.size())
    throw Error (InvalidRange, method, "ichan=%d >= nchan=%d", 
		 ichan, stokes.size());
}
