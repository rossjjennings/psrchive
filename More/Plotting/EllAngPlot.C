#include "Pulsar/EllAngPlotter.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"

Pulsar::EllAngPlotter::EllAngPlotter ()
{
  range = 90;
}

void Pulsar::EllAngPlotter::get_angles (const Archive* data,
					vector< Estimate<double> >& posang)
{
  const Integration* subint = data->get_Integration(isubint);
  Reference::To<const PolnProfile> profile = subint->new_PolnProfile(ichan);
  profile->get_ellipticity (posang, threshold);
}

//! Return the label for the y-axis
std::string Pulsar::EllAngPlotter::get_ylabel (const Archive* data)
{
  return "Ellipticity Angle (degrees)";
}
