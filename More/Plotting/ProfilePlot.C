/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfilePlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/SquareWave.h"

#include <cpgplot.h>

#include <fstream>
using namespace std;

Pulsar::ProfilePlot::ProfilePlot ()
{
  plot_cal_transitions = true;
  outlier_threshold = 0.0;
  plotter.set_pen( get_pen() );
}

TextInterface::Parser* Pulsar::ProfilePlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::ProfilePlot::get_profiles (const Archive* data)
{
  plotter.profiles.push_back( get_Profile (data, isubint, ipol, ichan) );
}

//! Derived classes must draw in the current viewport
void Pulsar::ProfilePlot::draw (const Archive* data)
{
  FluxPlot::draw (data);

  if (plot_cal_transitions && data->get_type() != Signal::Pulsar)
    draw_transitions (plotter.profiles[0]);
}

/*!  Plots the calibrator hi/lo levels using the transitions
 determined by Profile::find_transitions.  Plots in the currently open
 pgplot device using the current viewport and window.  */
void Pulsar::ProfilePlot::draw_transitions (const Profile* profile)
{
  int hightolow, lowtohigh, buffer;
  profile->find_transitions (hightolow, lowtohigh, buffer);

  double mean_hi, var_hi;
  double mean_lo, var_lo;

  if (outlier_threshold)
  {
    SquareWave estimator;
    estimator.set_outlier_threshold (outlier_threshold);
    
    Reference::To<PhaseWeight> high_mask;
    high_mask = estimator.get_mask (profile, true,
				    lowtohigh + buffer,
				    hightolow - buffer);

    high_mask->stats (profile, &mean_hi, &var_hi);
    	
    Reference::To<PhaseWeight> low_mask;    
    low_mask = estimator.get_mask (profile, false,
				   hightolow + buffer,
				   lowtohigh - buffer);

    low_mask->stats (profile, &mean_lo, &var_lo);

#if _DEBUG
    ofstream out ("transitions.txt");
    for (unsigned i=0; i < high_mask->get_nbin(); i++)
      out << i
	  << " " << high_mask->get_weights()[i]
	  << " " << low_mask->get_weights()[i]
	  << endl;
#endif
  }    
  else
  {
    profile->stats (&mean_hi, &var_hi, 0,
		    lowtohigh + buffer,
		    hightolow - buffer);

    profile->stats (&mean_lo, &var_lo, 0,
		    hightolow + buffer,
		    lowtohigh - buffer);
  }
  
  int st_low = 0;
  if (lowtohigh < hightolow)
    st_low = 1;

  float m_hi = mean_hi;
  float m_lo = mean_lo;
  float sigma_hi = sqrt(var_hi);
  float sigma_lo = sqrt(var_lo);

  float hip[3] = { m_hi, m_hi-sigma_hi, m_hi+sigma_hi };
  float lop[3] = { m_lo, m_lo-sigma_lo, m_lo+sigma_lo };

  // solid lines for the means, dashed lines for mean +/- 1-sigma
  int line_style[3] = { 1, 2, 2 };

  int colour=0, line=0;

  cpgqci(&colour);
  cpgqls(&line);

  float nbin = profile->get_nbin();

  float xp[2];
  xp[st_low] = float(hightolow)/nbin;
  xp[!st_low] = float(lowtohigh)/nbin;

  int cp[2];
  cp[st_low] = 3;
  cp[!st_low] = 2;

  float space = float(buffer)/nbin;

  // Added by DS, add xoff(set) to repeat if we are zooming outside 0 to 1

  float sx, ex;
  get_frame()->get_x_scale()->get_range( sx, ex );

  for( int xoff = int(sx)-1; xoff < int(ex)+1; xoff ++ )
  {
    for (int i=0; i<3; i++)
    {
      float yp[2];
      yp[st_low] = hip[i];
      yp[!st_low] = lop[i];

      cpgsci (cp[0]);
      cpgsls (line_style[i]);
      cpgmove (xp[1]+space-1.0 + xoff,yp[0]);
      cpgdraw (xp[0]-space + xoff,yp[0]);

      cpgsci (cp[1]);
      cpgsls (line_style[i]);
      cpgmove (xp[0]+space + xoff,yp[1]);
      cpgdraw (xp[1]-space + xoff,yp[1]);

      cpgsci (cp[0]);
      cpgsls (line_style[i]);
      cpgmove (xp[1]+space + xoff,yp[0]);
      cpgdraw (xp[0]-space+1.0 + xoff,yp[0]);
    }
  }

  // restore the colour and line attributes
  cpgsls(line);
  cpgsci(colour);
}


