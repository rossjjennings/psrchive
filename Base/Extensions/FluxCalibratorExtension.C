/***************************************************************************
 *
 *   Copyright (C) 2004-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorType.h"
#include "templates.h"

#if _DEBUG
#include <iostream>
using namespace std;
#endif

//! Default constructor
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension ()
  : CalibratorExtension ("FluxCalibratorExtension")
{
  type = Calibrator::Type::factory ("Flux");
  scale_available = false;
}

//! Copy constructor
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension
(const FluxCalibratorExtension& copy)
  : CalibratorExtension (copy)
{
  type = Calibrator::Type::factory ("Flux");
  operator = (copy);
}

//! Operator =
const Pulsar::FluxCalibratorExtension&
Pulsar::FluxCalibratorExtension::operator= (const FluxCalibratorExtension& fc)
{
  if (this == &fc)
    return *this;

  CalibratorExtension::operator= (fc);
  S_cal = fc.S_cal;
  S_sys = fc.S_sys;

  scale_available = fc.scale_available;

  scale = fc.scale;
  ratio = fc.ratio;

  return *this;
}

//! Destructor
Pulsar::FluxCalibratorExtension::~FluxCalibratorExtension ()
{
}

//! Set the number of frequency channels
void Pulsar::FluxCalibratorExtension::set_nchan (unsigned nchan)
{
  CalibratorExtension::set_nchan( nchan );

  S_cal.resize( nchan );
  S_sys.resize( nchan );
  scale.resize( nchan );
  ratio.resize( nchan );
}

//! Get the number of frequency channels
unsigned int Pulsar::FluxCalibratorExtension::get_nchan( void ) const
{
  return S_cal.size();
}

void Pulsar::FluxCalibratorExtension::remove_chan (unsigned first, unsigned last)
{
  CalibratorExtension::remove_chan (first, last);
  remove (S_cal, first, last);
  remove (S_sys, first, last);
  remove (scale, first, last);
  remove (ratio, first, last);
}

//! Set the number of frequency channels
void Pulsar::FluxCalibratorExtension::set_nreceptor (unsigned nreceptor)
{
  if (nreceptor == 0)
    throw Error (InvalidParam,"Pulsar::FluxCalibratorExtension::set_nreceptor",
		 "cannot set nreceptor to 0");

  for (unsigned ichan=0; ichan < S_cal.size(); ichan++)
  {
    S_cal[ichan].resize( nreceptor );
    S_sys[ichan].resize( nreceptor );
    scale[ichan].resize( nreceptor );
    ratio[ichan].resize( nreceptor );
  }
}

unsigned Pulsar::FluxCalibratorExtension::get_nreceptor () const
{
  if (S_cal.size())
    return S_cal[0].size();
  return 0;
}

using namespace Pulsar;

void FluxCalibratorExtension::set_S_sys (unsigned ichan, unsigned ireceptor,
					 const Estimate<double>& _S_sys)
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::set_S_sys");
  S_sys[ichan].at(ireceptor) = _S_sys;
}

Estimate<double> 
FluxCalibratorExtension::get_S_sys (unsigned ichan, unsigned ireceptor) const
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::get_S_sys");
  return S_sys[ichan].at(ireceptor);
}

std::vector< std::vector< Estimate<double> > >
FluxCalibratorExtension::get_S_sys () const
{
  return S_sys;
}

void FluxCalibratorExtension::set_S_cal (unsigned ichan, unsigned ireceptor,
					 const Estimate<double>& _S_cal)
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::set_S_cal");
  S_cal[ichan].at(ireceptor) = _S_cal;
}

Estimate<double>
FluxCalibratorExtension::get_S_cal (unsigned ichan, unsigned ireceptor) const
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::get_S_cal");
  return S_cal[ichan].at(ireceptor);
}

std::vector< std::vector< Estimate<double> > >
FluxCalibratorExtension::get_S_cal () const
{
  return S_cal;
}


void FluxCalibratorExtension::set_scale (unsigned ichan, unsigned ireceptor,
					 const Estimate<double>& _scale)
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::set_scale");
  scale[ichan].at(ireceptor) = _scale;
}

Estimate<double>
FluxCalibratorExtension::get_scale (unsigned ichan, unsigned ireceptor) const
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::get_scale");
  return scale[ichan].at(ireceptor);
}

void FluxCalibratorExtension::set_gain_ratio (unsigned ichan,
					      unsigned ireceptor,
					      const Estimate<double>& _ratio)
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::set_gain_ratio");
  ratio[ichan].at(ireceptor) = _ratio;
}

Estimate<double>
FluxCalibratorExtension::get_gain_ratio (unsigned ichan, 
					 unsigned ireceptor) const
{
  range_check (ichan, "Pulsar::FluxCalibratorExtension::get_gain_ratio");
  return ratio[ichan].at(ireceptor);
}

TextInterface::Parser* FluxCalibratorExtension::get_interface()
{
  return new Interface( this );
}

unsigned FluxCalibratorExtension::get_nparam () const
{
  // S_sys and S_cal
  unsigned nparam = get_nreceptor() * 2;

  // scale and gain_ratio
  if (scale_available)
    nparam += get_nreceptor() * 2;

  return nparam;
}
 
Estimate<float> FluxCalibratorExtension::get_Estimate ( unsigned iparam, unsigned ichan ) const
{
  unsigned jparam = iparam / get_nreceptor();
  unsigned ireceptor = iparam % get_nreceptor();

  switch (jparam)
  {
    case 0: return get_S_sys (ichan, ireceptor);
    case 1: return get_S_cal (ichan, ireceptor);
    case 2: return get_scale (ichan, ireceptor);
    case 3: return get_gain_ratio (ichan, ireceptor);
    default:
      throw Error (InvalidParam, "FluxCalibratorExtension::get_Estimate",
                   "iparam=%u,nreceptor=%u -> jparam=%u,ireceptor=%u", 
                    iparam, get_nreceptor(), jparam, ireceptor);
  }
}

void FluxCalibratorExtension::set_Estimate ( unsigned iparam, unsigned ichan, const Estimate<float>& val)
{
  unsigned jparam = iparam / get_nreceptor();
  unsigned ireceptor = iparam % get_nreceptor();

  switch (jparam)
  {
    case 0: set_S_sys (ichan, ireceptor, val); return;
    case 1: set_S_cal (ichan, ireceptor, val); return;
    case 2: set_scale (ichan, ireceptor, val); return;
    case 3: set_gain_ratio (ichan, ireceptor, val); return;
    default:
      throw Error (InvalidParam, "FluxCalibratorExtension::get_Estimate",
                   "iparam=%u,nreceptor=%u -> jparam=%u,ireceptor=%u",
                    iparam, get_nreceptor(), jparam, ireceptor);
  }
}

bool FluxCalibratorExtension::get_valid (unsigned ichan) const
{
  return get_weight (ichan) != 0;
}

void FluxCalibratorExtension::set_valid (unsigned ichan, bool valid)
{
  if (!valid)
    set_weight (ichan, 0);
  else
    set_weight (ichan, 1.0);
}
 
