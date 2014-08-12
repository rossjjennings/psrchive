/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TimeScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Telescope.h"
#include "pairutil.h"

#include <iostream>
using namespace std;

Pulsar::TimeScale::TimeScale ()
{
  units = Time;
  origin_norm = 0;
}

void Pulsar::TimeScale::init (const Archive* data)
{
  if (!data)
    return;

  unsigned nsubint = data->get_nsubint();

  // PlotScale::num_indeces
  num_indeces = nsubint;

  const IntegrationOrder* order = data->get<IntegrationOrder>();
  if (order)
  {
    set_minmax( order->get_Index(0), order->get_Index(nsubint-1) );

    label = order->get_IndexState();
    if (!order->get_Unit().empty())
      label += " (" + order->get_Unit() + ")";

    return;
  }

  // subint,mjd,time,ha

  if (units == Subint)
  {
    label = "Sub-integration Index";
    set_minmax( 0, nsubint );
    return;
  }

  if (units == Time)
  {
    double range = (data->end_time() - data->start_time()).in_days();

    const float mjd_hours = 1.0 / 24.0;
    const float mjd_minutes = mjd_hours / 60.0;
    const float mjd_seconds = mjd_minutes / 60.0;
    const float mjd_ms = mjd_seconds / 1e3;

    label = "Time ";
    if (range > 1.0)
    {
      label += "(days)";
      set_minmax (0, range);
    }
    else if (range > mjd_hours)
    {
      label += "(hours)";
      set_minmax (0, range/mjd_hours);
    }
    else if (range > mjd_minutes)
    {
      label += "(minutes)";
      set_minmax (0, range/mjd_minutes);
    }
    else if (range > mjd_seconds)
    {
      label += "(seconds)";
      set_minmax (0, range/mjd_seconds);
    }
    else
    {
      label += "(ms)";
      set_minmax (0, range/mjd_ms);
    }
  }

  if (units == HourAngle)
  {
    const Telescope* tel = data->get<Telescope>();
    if (tel)
    {
      Reference::To<Directional> dir = tel->get_Directional();
      dir->set_source_coordinates(data->get_coordinates());
      label = "Hour Angle (hours)";

      double ha0, ha1;
      dir->set_epoch(data->start_time());
      ha0 = dir->get_hour_angle();
      dir->set_epoch(data->end_time());
      ha1 = dir->get_hour_angle();

      ha0 *= 12.0 / M_PI;
      ha1 *= 12.0 / M_PI;

      // In case we're circumpolar
      if (ha1<ha0) ha1 += 24.0;

      set_minmax(ha0, ha1);
    }

    // TODO : use Pointing extension if available?
  }
}

void Pulsar::TimeScale::get_indeces (const Archive* data, 
				     unsigned& min, unsigned& max) const
{
  PlotScale::get_indeces (data->get_nsubint(), min, max);
}

std::string Pulsar::TimeScale::get_label () const
{
  return label;
}

void Pulsar::TimeScale::get_ordinates (const Archive* data,
				       std::vector<float>& x_axis) const
{
  x_axis.resize (data->get_nsubint());

  float min=0, max=0;
  get_minmax( min, max );

  for (unsigned ibin = 0; ibin < x_axis.size(); ibin++)
    x_axis[ibin] = min + (max-min) * double(ibin) / (x_axis.size()-1);
}



std::ostream& Pulsar::operator << (std::ostream& os, TimeScale::Units units)
{
  switch (units) {
  case TimeScale::Time:
    return os << "time";
  case TimeScale::HourAngle:
    return os << "ha";
  case TimeScale::MJD:
    return os << "mjd";
  case TimeScale::Subint:
    return os << "subint";
  default:
    return os << "unknown";
  }
}

std::istream& Pulsar::operator >> (std::istream& is, TimeScale::Units& units)
{
  std::streampos pos = is.tellg();
  std::string unit;
  is >> unit;

  if (unit == "time")
    units = TimeScale::Time;
  else if (unit == "ha")
    units = TimeScale::HourAngle;
  else if (unit == "mjd")
    units = TimeScale::MJD;
  else if (unit == "subint")
    units = TimeScale::Subint;
  else
    is.setstate(std::istream::failbit);

  return is;
}

Pulsar::TimeScale::Interface::Interface (TimeScale* instance)
{
  if (instance)
    set_instance (instance);

  import( PlotScale::Interface() );

  add( &PlotScale::get_index_range,
       &PlotScale::set_index_range,
       "subint", "Sub-integration index range" );

  add( &TimeScale::get_units,
       &TimeScale::set_units,
       "unit", "Units on time axis (subint,mjd,time,ha)");

  add( &TimeScale::get_origin_norm,
       &TimeScale::set_origin_norm,
       "origin", "Offset of origin on time axis" );
}
