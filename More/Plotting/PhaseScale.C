#include "Pulsar/PhaseScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

Pulsar::PhaseScale::PhaseScale ()
{
  units = Turns;
  origin_norm = 0;
}

void Pulsar::PhaseScale::get_range_bin (const Archive* data, 
					unsigned& min, unsigned& max)
{
  float x_min = 0.0;
  float x_max = data->get_nbin();
  get_range (x_min, x_max);
  min = (unsigned) x_min;
  max = (unsigned) x_max;
}

void Pulsar::PhaseScale::get_range_units (const Archive* data,
					  float& min, float& max)
{
  min = 0;
  max = 1.0;
  get_range (min, max);

  float scale = 1.0;

  if (units == Milliseconds)
    scale = data->get_Integration(0)->get_folding_period() * 1e3;

  else if (units == Radians)
    scale = 2.0 * M_PI;

  else if (units == Degrees)
    scale = 180.0;

  min += origin_norm;
  max += origin_norm;

  min *= scale;
  max *= scale;
}

//! Get the default label for the x axis
std::string Pulsar::PhaseScale::get_label ()
{
  switch (units) {
  case Turns: return "Pulse Phase";
  case Degrees: return "Phase (deg.)";
  case Radians: return "Phase (rad.)";
  case Milliseconds: return "Time (ms)";
  }
}

std::ostream& Pulsar::operator << (std::ostream& os, PhaseScale::Units units)
{
  switch (units) {
  case PhaseScale::Turns:
    return os << "turn";
  case PhaseScale::Degrees:
    return os << "deg";
  case PhaseScale::Radians:
    return os << "rad";
  case PhaseScale::Milliseconds:
    return os << "ms";
  }
}

std::istream& Pulsar::operator >> (std::istream& is, PhaseScale::Units& units)
{
  std::streampos pos = is.tellg();
  std::string unit;
  is >> unit;

  if (unit == "turn")
    units = PhaseScale::Turns;
  else if (unit == "deg")
    units = PhaseScale::Degrees;
  else if (unit == "rad")
    units = PhaseScale::Radians;
  else if (unit == "ms")
    units = PhaseScale::Milliseconds;
  else
    is.setstate(std::istream::failbit);

  return is;
}
