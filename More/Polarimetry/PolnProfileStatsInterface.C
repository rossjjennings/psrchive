/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnProfileStatsInterface.h"
#include "Pulsar/ProfileStatsInterface.h"

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
       "sumI", "Total flux in on-pulse phase bins" );

  add( &PolnProfileStats::get_total_polarized,
       "sumP", "Total polarized flux of on-pulse phase bins" );

  add( &PolnProfileStats::get_total_linear,
       "sumL", "Total linearly polarized flux of on-pulse phase bins" );

  add( &PolnProfileStats::get_total_circular,
       "sumV", "Total Stokes V of on-pulse phase bins" );

  add( &PolnProfileStats::get_total_abs_circular,
       "sumC", "Total circularly polarized flux |V| of on-pulse phase bins" );

  add( &PolnProfileStats::get_total_determinant,
       "sumS", "Total coherency matrix determinant of on-pulse phase bins" );

  add (&PolnProfileStats::get_linear_variance,
       "varL", "Variance of the off-pulse linearly polarized flux");
}


