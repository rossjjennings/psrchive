/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

using namespace std;

Pulsar::PhaseScale::PhaseScale ()
{
  units = Phase::Turns;

  origin = 0;
  origin_units = units;
  origin_norm = 0;
}

void Pulsar::PhaseScale::init (const Archive* data)
{
  if (!data)
    return;

  // PlotScale::num_indeces
  num_indeces = data->get_nbin();

  float origin_scale = get_scale (data, origin_units);
  origin_norm = origin / origin_scale;

  // cerr << "PhaseScale::init scale=" << origin_scale << " origin=" << origin << endl;

  float min = origin_norm;
  float max = 1 + origin_norm;

  float scale = get_scale (data);

  min *= scale;
  max *= scale;

  set_minmax (min, max);
}

void Pulsar::PhaseScale::get_indeces (const Archive* data, 
				      unsigned& min, unsigned& max) const
{
  bool cyclic = true;
  PlotScale::get_indeces (data->get_nbin(), min, max, cyclic);
}


void Pulsar::PhaseScale::get_ordinates (const Archive* data,
					std::vector<float>& axis) const
{
  float scale = 1.0;
 
  if (data != 0) {
    axis.resize (data->get_nbin());
    scale = get_scale (data);
  }

  for (unsigned ibin = 0; ibin < axis.size(); ibin++)
    axis[ibin] = scale * (origin_norm + (float(ibin) + 0.5) / axis.size());
}

float Pulsar::PhaseScale::get_scale (const Archive* data) const
{
  return get_scale (data, units);
}

float Pulsar::PhaseScale::get_scale (const Archive* data, Phase::Unit unit) const
{
  double period_in_seconds = 1.0;
  double gate = 1.0;

  if (data)
  {
    period_in_seconds = data->get_Integration(0)->get_folding_period();
    gate = data->get_Integration(0)->get_gate_duty_cycle();
  }

  switch (unit)
    {
    case Phase::Turns:
      return gate;
      
    case Phase::Degrees:
      return 360.0 * gate;

    case Phase::Bins:
      return data->get_nbin();

    case Phase::Milliseconds:
      return gate * period_in_seconds * 1e3;

    case Phase::Seconds:
      return gate * period_in_seconds;

    case Phase::Minutes:
      return gate * period_in_seconds / 60.0;

    case Phase::Hours:
      return gate * period_in_seconds / 3600.0;

    case Phase::Radians:
      return gate * 2.0 * M_PI;

    default:
      return gate;
    }
}

//! Get the default label for the x axis
std::string Pulsar::PhaseScale::get_label () const
{
  switch (units) {
  case Phase::Turns: return "Pulse Phase";
  case Phase::Degrees: return "Pulse Longitude (deg.)";
  case Phase::Bins: return "Bin Number";
  case Phase::Milliseconds: return "Time (ms)";
  case Phase::Seconds: return "Time (s)";
  case Phase::Minutes: return "Time (min.)";
  case Phase::Hours: return "Time (hr.)";
  case Phase::Radians: return "Phase (rad.)";
  default: return "Unknown";
  }
}

