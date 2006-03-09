#include "Pulsar/PosAngPlotter.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/PolnProfile.h"

void Pulsar::PosAngPlotter::get_angles (const Archive* data,
					vector< Estimate<double> >& posang)
{
  Reference::To<const PolnProfile> profile = new_Stokes (data,isubint,ichan);
  profile->get_orientation (posang, threshold);
}

//! Return the label for the y-axis
std::string Pulsar::PosAngPlotter::get_ylabel (const Archive* data)
{
  return "Position Angle (degrees)";
}
