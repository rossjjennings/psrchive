#include "Pulsar/PosAngPlotter.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

void Pulsar::PosAngPlotter::get_angles (const Archive* data,
					vector< Estimate<double> >& posang)
{
  const Integration* subint = data->get_Integration(isubint);
  Reference::To<const PolnProfile> profile = subint->new_PolnProfile(ichan);
  profile->get_orientation (posang, threshold);
}

//! Return the label for the y-axis
std::string Pulsar::PosAngPlotter::get_ylabel (const Archive* data)
{
  return "Position Angle (degrees)";
}
