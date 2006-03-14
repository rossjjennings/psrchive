#include "Pulsar/PhaseScale.h"

Pulsar::PhaseScale::PhaseScale ()
{
  scale = Turns;
  origin_norm = 0;
}

//! Get the default label for the x axis
std::string Pulsar::PhaseScale::get_label ()
{
  switch (scale) {
  case Turns: return "Pulse Phase";
  case Degrees: return "Phase (deg.)";
  case Radians: return "Phase (rad.)";
  case Milliseconds: return "Time (ms)";
  }
}

std::ostream& Pulsar::operator << (std::ostream& os, PhaseScale::Scale scale)
{
  switch (scale) {
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

std::istream& Pulsar::operator >> (std::istream& is, PhaseScale::Scale& scale)
{
  std::streampos pos = is.tellg();
  std::string unit;
  is >> unit;

  if (unit == "turn")
    scale = PhaseScale::Turns;
  else if (unit == "deg")
    scale = PhaseScale::Degrees;
  else if (unit == "rad")
    scale = PhaseScale::Radians;
  else if (unit == "ms")
    scale = PhaseScale::Milliseconds;
  else
    is.setstate(std::istream::failbit);

  return is;
}
