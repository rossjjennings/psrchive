/***************************************************************************
 *
 *   Copyright (C) 2006 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PowerSpectra.h"
#include "Pulsar/PlotFrame.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/HasPen.h"

#include "Physical.h"
#include "median_smooth.h"

#include <cpgplot.h>

#include <float.h>

using namespace std;

Pulsar::PowerSpectra::PowerSpectra ()
{
  isubint = ipol = 0;
  draw_lines = true;
  skip_zapped = false;
  median_window = 0;
  logarithmic = false;

  get_frame()->get_y_scale()->set_buf_norm(0.05);
}
 
TextInterface::Parser* Pulsar::PowerSpectra::get_interface ()
{
  return new Interface (this);
}

//! Derived classes must compute the minimum and maximum values (y-axis)
void Pulsar::PowerSpectra::prepare (const Archive* data)
{
  spectra.clear();
  get_spectra (data);

  if (!spectra.size())
    throw Error (InvalidState, "Pulsar::PowerSpectra::prepare",
		 "Spectra array empty after call to get_spectra");

  if (logarithmic)
  {
    // logarithmic axis
    frame->get_y_axis()->add_opt ('L');
    // vertical labels
    frame->get_y_axis()->add_opt ('V');
    // exponential notation
    frame->get_y_axis()->add_opt ('2');
    // increase the space between the label and the axis
    frame->get_y_axis()->set_displacement (3.0);
  }

  unsigned i_min, i_max;
  get_scale()->get_indeces (data, i_min, i_max);

  if (i_min >= i_max)
    throw Error (InvalidState, "Pulsar::PowerSpectra::prepare",
		 "frequency scale i_min=%d >= i_max=%d", i_min, i_max);

  float min = FLT_MAX;
  float max = -FLT_MAX;

  for (unsigned iprof=0; iprof < spectra.size(); iprof++)
  {
    if (median_window)
      fft::median_smooth (spectra[iprof], median_window);

    if (logarithmic)
    {
      double log_base = 1.0/log(10.0);

      unsigned nchan = data->get_nchan();
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
        if (spectra[iprof][ichan] <= 0)
          spectra[iprof][ichan] = -FLT_MAX;
        else
          spectra[iprof][ichan] = log_base * log( spectra[iprof][ichan] );
      }
    }

    for (unsigned ichan=i_min; ichan < i_max; ichan++)
      if (spectra[iprof][ichan] != 0 && spectra[iprof][ichan] != -FLT_MAX)
      {
        min = std::min( min, spectra[iprof][ichan] );
        max = std::max( max, spectra[iprof][ichan] );
      }

    for (unsigned ichan=i_min; ichan < i_max; ichan++)
      if (spectra[iprof][ichan] == -FLT_MAX)
        spectra[iprof][ichan] = min;

  }

  if (verbose)
    cerr << "Pulsar::PowerSpectra::prepare"
      " min=" << min << " max=" << max << endl;

  get_frame()->get_y_scale()->set_minmax (min, max);
}


//! Derived classes must draw in the current viewport
void Pulsar::PowerSpectra::draw (const Archive* data)
{
  get_scale()->get_ordinates (data, frequencies);

  HasPen* has_pen = dynamic_cast<HasPen*> (this);
  
  for (unsigned iprof=0; iprof < spectra.size(); iprof++)
  {
    if (has_pen)
    {
      has_pen->get_pen()->setup();
    }
    else
    {
      if (plot_sci.size() == spectra.size())
        cpgsci (plot_sci[iprof]);
      else
        cpgsci (iprof+1);
  
      if (plot_sls.size() == spectra.size())
        cpgsls (plot_sls[iprof]);
      else
        cpgsls (iprof+1);
    }

    draw (spectra[iprof]);
  }
}

// Plots the bandpass but places splodges where weights are zero.
void cpgbpass(int nchan, const float* xaxis, const float* bpass)
{
  // determine minimum and maximum - max not used at present

// find out what the PGPLOT CI is now? 
  int pgplot_ci; 
  cpgqci(&pgplot_ci);

  float ymin = bpass[0];
  float ymax = bpass[0];
  for (int i=1;i<nchan;i++){
    if (bpass[i]<ymin) ymin=bpass[i];
    if (bpass[i]>ymax) ymax=bpass[i];
  }
  // draw splodges at the bottom in dark grey 
  //fprintf(stderr,"Setting CI to 14\n");
  cpgscr(16,0.2,0.2,0.2);
  cpgsci(16);
  int istart=-1; // zero region hasn't started
  int iend=-1;   // zero region hasn't ended
  for (int i=0;i<nchan;i++){
    // find the first zeroed bpass
    if ((fabs(bpass[i]-ymin)/(ymax-ymin)<0.001)&&(istart == (-1))) {
      //cpgpt1(xaxis[i],bpass[i],17);
      istart = i;
    }
    // determine if it is still zero
    if ((fabs(bpass[i]-ymin)/(ymax-ymin)<0.001)&&(istart != (-1))) {
      //cpgpt1(xaxis[i],bpass[i],17);
      iend = i;
    }    
    // once it returns to zero - plot the grey region and reset
    if ((fabs(bpass[i]-ymin)/(ymax-ymin)>=0.001)) {
      //cpgpt1(xaxis[i],bpass[i],17);
      cpgsfs(1);
      cpgrect(xaxis[istart],xaxis[iend],ymin,ymax);
      istart=-1;
      iend=-1;
    }

  }
  //draw bandpass between non-null points with original pgplot CI
  cpgsci(pgplot_ci);
  for (int i=0;i<nchan-1;i++){
    if (fabs(bpass[i]-ymin)/(ymax-ymin)>0.001 
	&& fabs(bpass[i+1]-ymin)/(ymax-ymin)>0.001) {
      cpgmove(xaxis[i],bpass[i]);
      cpgdraw(xaxis[i+1],bpass[i+1]);
    }
  }
}

//! draw the profile in the current viewport and window
void Pulsar::PowerSpectra::draw (const vector<float>& data) const
{
  if (verbose)
  {
    float x_min, x_max;
    float y_min, y_max;
    cpgqwin (&x_min, &x_max, &y_min, &y_max);
    
    cerr << "Pulsar::PowerSpectra::draw xmin=" << x_min << " xmax=" << x_max
	 << " ymin=" << y_min << " ymax=" << y_max << endl;
  }

  const float* x = &frequencies[0];
  const float* y = &data[0];

  if (get_frame()->get_transpose())
    swap (x,y);

  if (draw_lines)
  {
    if (skip_zapped)
      cpgbpass(data.size(), x, y);
    else
      cpgline (data.size(), x, y);
  }
  else
  {
    cpgpt (data.size(), x, y, -1);
  }
}



//! Return the label for the y-axis
std::string Pulsar::PowerSpectra::get_ylabel (const Archive* data)
{
  if (data->get_scale() == Signal::Jansky)
    return "Flux Density (mJy)";
  else if (logarithmic)
    return "Log (Flux)";
  else
    return "Relative Flux Units";
}

