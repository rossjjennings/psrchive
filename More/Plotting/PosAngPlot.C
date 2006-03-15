#include "Pulsar/PosAngPlot.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/PolnProfile.h"

void Pulsar::PosAngPlot::get_angles (const Archive* data)
{
  Reference::To<const PolnProfile> profile = get_Stokes (data,isubint,ichan);
  profile->get_orientation (angles, threshold);
}

//! Return the label for the y-axis
std::string Pulsar::PosAngPlot::get_ylabel (const Archive* data)
{
  return "Position Angle (degrees)";
}
