/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Poincare.h"
#include "Pulsar/PolnProfile.h"

#include "Plot3D.h"

#include <cpgplot.h>

#include <algorithm>
#include <math.h>

using namespace std;

Pulsar::Poincare::Poincare ()
{
  longitude = 45;
  latitude = 25;
  animate = false;
}

//! Plot in the current viewport
void Pulsar::Poincare::plot (const Archive* data)
{
  Reference::To<const PolnProfile> profile = get_Stokes (data, isubint, ichan);

  unsigned i_min, i_max, nbin = profile->get_nbin();

  get_phase_scale()->get_range (nbin, i_min, i_max);

  float max = 0;

  for (unsigned ipol=1; ipol < 4; ipol++) {
    const float* amps = profile->get_Profile(ipol)->get_amps();

    float max_amp = *max_element (amps+i_min, amps+i_max);
    float min_amp = *min_element (amps+i_min, amps+i_max);

    float abs_min = fabs( min_amp );
    max_amp = std::max( abs_min, max_amp );
    max = std::max (max_amp, max);
  }

  max *= 1.1;

  cpgpap (0.0, 1.0);
  cpgswin (-max,max,-max,max);

  pgplot::Plot3D volume;
  Cartesian origin (0,0,0);
  Cartesian x0 (max,0,0);
  Cartesian y0 (0,max,0);
  Cartesian z0 (0,0,max);

  float textsep = 0.05;

  unsigned nplot = 1;

  if (animate)
    nplot = 360;

  float lat = latitude;
  float lon = longitude;

  for (unsigned iplot=0; iplot < nplot; iplot++) {

    volume.set_camera (lon, lat);
    
    // draw and label the axis
    cpgsci (2);
    volume.arrow (origin, x0);
    volume.text (x0*(1+textsep), "Q");
    
    cpgsci (3);
    volume.arrow (origin, y0);
    volume.text (y0*(1+textsep), "U");
    
    cpgsci (4);
    volume.arrow (origin, z0);
    volume.text (z0*(1+textsep), "V");
    
    cpgsci (1);
    for (unsigned ibin=i_min; ibin < i_max; ibin++) {

      Cartesian p ( profile->get_Profile(1)->get_amps()[ibin],
		    profile->get_Profile(2)->get_amps()[ibin],
		    profile->get_Profile(3)->get_amps()[ibin] );
      
      if (ibin == i_min)
	volume.move (p);
      else
	volume.draw (p);
      
    }

    if (nplot > 1)
      cpgpage ();

    lon += 4;
    lat += 1;

  }

}

