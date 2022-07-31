/***************************************************************************
 *
 *   Copyright (C) 2004-2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FluxCalibratorExtension.h"
#include "Pulsar/CalibratorType.h"
#include "templates.h"

#if _DEBUG
#include <iostream>
#endif

using namespace std;

//! Default constructor
Pulsar::FluxCalibratorExtension::FluxCalibratorExtension ()
  : CalibratorExtension ("FluxCalibratorExtension")
{
  type = Calibrator::Type::factory ("Flux");
  scale_available = false;
  nreceptor = 0;
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

  nreceptor = fc.nreceptor;
  scale_available = fc.scale_available;
  solution = fc.solution;

  CalibratorExtension::operator= (fc);

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
  solution.resize( nchan );
}

//! Get the number of frequency channels
unsigned Pulsar::FluxCalibratorExtension::get_nchan () const
{
  return solution.size();
}

void Pulsar::FluxCalibratorExtension::remove_chan (unsigned first,
						   unsigned last)
{
  CalibratorExtension::remove_chan (first, last);
  remove (solution, first, last);
}

//! Set the number of frequency channels
void Pulsar::FluxCalibratorExtension::set_nreceptor (unsigned _nreceptor)
{
  if (_nreceptor == 0)
    throw Error (InvalidParam,"Pulsar::FluxCalibratorExtension::set_nreceptor",
		 "cannot set nreceptor to 0");

  nreceptor = _nreceptor;
  
  for (unsigned ichan=0; ichan < solution.size(); ichan++)
    solution[ichan].resize( nreceptor );
}

unsigned Pulsar::FluxCalibratorExtension::get_nreceptor () const
{
  return nreceptor;
}

using namespace Pulsar;

FluxCalibratorExtension::Solution* 
FluxCalibratorExtension::get_solution (unsigned c)
{
  range_check (c, "Pulsar::FluxCalibratorExtension::set_solution");
  return &solution[c];
}

const FluxCalibratorExtension::Solution* 
FluxCalibratorExtension::get_solution (unsigned c) const
{
  range_check (c, "Pulsar::FluxCalibratorExtension::set_solution");
  return &solution[c];
}

unsigned FluxCalibratorExtension::Solution::get_nreceptor () const
{
  return S_cal.size();
}

void FluxCalibratorExtension::Solution::resize (unsigned nreceptor)
{
  S_cal.resize( nreceptor );
  S_sys.resize( nreceptor );
  scale.resize( nreceptor );
  ratio.resize( nreceptor );
}

void FluxCalibratorExtension::Solution::set_S_sys (unsigned ireceptor,
					           const Estimate<double>& _S_sys)
{
  S_sys.at(ireceptor) = _S_sys;
}

Estimate<double> 
FluxCalibratorExtension::Solution::get_S_sys (unsigned ireceptor) const
{
  return S_sys.at(ireceptor);
}

#if 0
std::vector< std::vector< Estimate<double> > >
FluxCalibratorExtension::get_S_sys () const
{
  return S_sys;
}
#endif

void FluxCalibratorExtension::Solution::set_S_cal (unsigned ireceptor,
					 const Estimate<double>& _S_cal)
{
  S_cal.at(ireceptor) = _S_cal;
}

Estimate<double>
FluxCalibratorExtension::Solution::get_S_cal (unsigned ireceptor) const
{
  return S_cal.at(ireceptor);
}

#if 0
std::vector< std::vector< Estimate<double> > >
FluxCalibratorExtension::get_S_cal () const
{
  return S_cal;
}
#endif

void FluxCalibratorExtension::Solution::set_scale (unsigned ireceptor,
					 const Estimate<double>& _scale)
{
  scale.at(ireceptor) = _scale;
}

Estimate<double>
FluxCalibratorExtension::Solution::get_scale (unsigned ireceptor) const
{
  return scale.at(ireceptor);
}

void FluxCalibratorExtension::Solution::set_gain_ratio ( unsigned ireceptor,
					      const Estimate<double>& _ratio)
{
  ratio.at(ireceptor) = _ratio;
}

Estimate<double>
FluxCalibratorExtension::Solution::get_gain_ratio (unsigned ireceptor) const
{
  return ratio.at(ireceptor);
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
    case 0: return get_solution(ichan)->get_S_sys (ireceptor);
    case 1: return get_solution(ichan)->get_S_cal (ireceptor);
    case 2: return get_solution(ichan)->get_scale (ireceptor);
    case 3: return get_solution(ichan)->get_gain_ratio (ireceptor);
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

#if _DEBUG
  cerr << "FluxCalibratorExtension::set_Estimate ichan=" << ichan
       << " ireceptor=" << ireceptor << " jparam=" << jparam
       << " val=" << val << endl;
#endif
  
  switch (jparam)
  {
    case 0: get_solution(ichan)->set_S_sys (ireceptor, val); return;
    case 1: get_solution(ichan)->set_S_cal (ireceptor, val); return;
    case 2: get_solution(ichan)->set_scale (ireceptor, val); return;
    case 3: get_solution(ichan)->set_gain_ratio (ireceptor, val); return;
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

void FluxCalibratorExtension::frequency_append (Archive* to,
						const Archive* from)
{
  const FluxCalibratorExtension* ext = from->get<FluxCalibratorExtension>();
  if (!ext)
    throw Error (InvalidState, "FluxCalibratorExtension::frequency_append",
		 "other Archive does not have a FluxCalibratorExtension");

  if (scale_available != ext->scale_available)
    throw Error (InvalidState, "FluxCalibratorExtension::frequency_append",
		 "incompatible scale_available this=%u other=%u",
		 scale_available, ext->scale_available);

  bool in_order = in_frequency_order (to, from);
  CalibratorExtension::frequency_append (ext, in_order);
  
  solution.insert ( in_order ? solution.end() : solution.begin(),
		    ext->solution.begin(), ext->solution.end() );
}

