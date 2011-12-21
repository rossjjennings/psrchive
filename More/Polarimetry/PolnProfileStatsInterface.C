/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfileStatsInterface.h"

using namespace std;

//! Override the default Estimate insertion operator
std::ostream& operator<< (std::ostream& ostr, const Estimate<double>& estimate)
{
  return ostr << estimate.val;
}

Pulsar::PolnProfileStats::Interface::Interface (PolnProfileStats* instance)
{
  if (instance)
    set_instance (instance);

  add( &PolnProfileStats::get_total_intensity,
       "sum_I", "Total flux in on-pulse phase bins" );

  add( &PolnProfileStats::get_total_polarized,
       "sum_p", "Total polarized flux of on-pulse phase bins" );

  add( &PolnProfileStats::get_total_linear,
       "sum_L", "Total linearly polarized flux of on-pulse phase bins" );

  add( &PolnProfileStats::get_total_circular,
       "sum_V", "Total Stokes V of on-pulse phase bins" );

  add( &PolnProfileStats::get_total_abs_circular,
       "sum_C", "Total circularly polarized flux of on-pulse phase bins" );

}


