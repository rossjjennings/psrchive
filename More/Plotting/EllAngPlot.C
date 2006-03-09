#include "Pulsar/EllAngPlotter.h"
#include "Pulsar/Polarization.h"
#include "Pulsar/PolnProfile.h"

Pulsar::EllAngPlotter::EllAngPlotter ()
{
  range = 90;
}

void Pulsar::EllAngPlotter::get_angles (const Archive* data,
					vector< Estimate<double> >& ell)
{
  Reference::To<const PolnProfile> profile = new_Stokes (data,isubint,ichan);
  profile->get_ellipticity (ell, threshold);
}

//! Return the label for the y-axis
std::string Pulsar::EllAngPlotter::get_ylabel (const Archive* data)
{
  return "Ellipticity Angle (degrees)";
}
