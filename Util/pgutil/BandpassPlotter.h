//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/pgutil/BandpassPlotter.h,v $
   $Revision: 1.5 $
   $Date: 2008/05/18 21:19:38 $
   $Author: straten $*/

#ifndef __fft_BandpassPlotter_h
#define __fft_BandpassPlotter_h

#include "templates.h"
#include <cpgplot.h>
#include <algorithm>

namespace fft {

  template<class Data, class Info>
  class BandpassPlotter {

    public:
    
    //! Default constructor
    BandpassPlotter ()
    { 
      user_max = user_min = 0; ignore_fraction = 0.0; 
      logarithmic = false; xlabel_ichan = false;
      title = "Original Bandpass";
    }

    //! Destructor
    virtual ~BandpassPlotter() { }

    //! Plot the data using the given information
    virtual void plot (Data* data, Info* info) const;

    //! Plot the dynamic spectrum using the given information
    template<class T>
    void plot (std::vector< std::vector<T> >& data, double timespan, Info* info) const;

    //! Perform some operation on the bandpass before plotting
    virtual void preprocess (std::vector<float>& bandpass) const { }

    //! Maximum value in plots
    void set_minmax (float min, float max)
    { user_min = min; user_max = max; }

    //! Ignore band edges
    float ignore_fraction;

    //! Use logarithmic axis
    bool logarithmic;

    //! Label x-axis with channel number
    bool xlabel_ichan;

    std::string title;

    protected:
      float user_min, user_max;

  };

}

void log (std::vector<float>& data)
{
  for (unsigned i=0; i<data.size(); i++)
    if (data[i] > 0)
      data[i] = log10 (data[i]);
    else
      data[i] = 0.0;
}

template<class Data, class Info>
void fft::BandpassPlotter<Data,Info>::plot (Data* data, Info* info) const
{
  unsigned ipol, npol = data->get_npol ();

  float min = 0;
  float max = 0;
  for (ipol=0; ipol<npol; ipol++)
  {
    std::vector<float> pband = data->get_passband (ipol);
    this->preprocess (pband);
    if (logarithmic)
      log (pband);
    minmax (pband, min, max, ipol);
  }
  
  if (user_min != user_max)
  {
    min = user_min;
    max = user_max;
  }

  double freq = info->get_centre_frequency();
  double bw = info->get_bandwidth();

  double fmin = freq - bw/2;
  double fmax = freq + bw/2;

  std::string xlabel = "Frequency (MHz)";

  if (xlabel_ichan)
  {
    fmin = 0;
    fmax = 1;
    xlabel = "Frequency channel";
  }

  cpgsci(1);
  cpgswin(fmin, fmax, min, max);

  unsigned nchan = 0;

  // std::cerr << "npol=" << npol << std::endl;

  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    std::vector<float> pband = data->get_passband (ipol);
    this->preprocess (pband);
    if (logarithmic)
      log (pband);

    nchan = pband.size();

    // std::cerr << "ipol=" << ipol << " nchan=" << nchan << std::endl;

    double fstep = bw/(nchan-1);
    if (xlabel_ichan)
      fstep = 1.0/nchan;
 
    cpgsci(ipol+2);
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      float x = fmin + ichan*fstep;
      float y = pband[ichan];

      // std::cerr << "ichan=" << ichan << " x=" << x << " y=" << y << std::endl;

      if (ichan == 0)
	cpgmove (x, y);
      else
	cpgdraw (x, y);
    }
  }

  if (xlabel_ichan)
  {
    fmax = nchan;
    cpgswin(fmin, fmax, min, max);
  }

  cpgsci (1);

  if (logarithmic)
  {
    cpgbox("BCNST", 0.0, 0, "BCNSTL2", 0.0, 0);
    cpglab(xlabel.c_str(), "Logarithmic Scale", title.c_str());
  }
  else
  {
    cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
    cpglab(xlabel.c_str(), "Linear Scale", title.c_str());
  }
}


template<class Data, class Info>
template<class T>
void fft::BandpassPlotter<Data,Info>::plot (std::vector< std::vector<T> >& data,
					    double timespan, Info* info) const
{
  unsigned ntime = data.size();
  unsigned nchan = data[0].size();

  std::vector<float> plotarray (ntime * nchan);

  float min = 0;
  float max = 0;

  unsigned jchan = 0;

  unsigned ignore_ichan = (unsigned) (ignore_fraction * nchan);

  for (unsigned itime=0; itime<ntime; itime++) {
    std::vector<float>& pband = data[itime];
    this->preprocess (pband);

    minmax (pband.begin()+ignore_ichan, pband.end()-ignore_ichan,
            min, max, itime);

    for (unsigned ichan=0; ichan<nchan; ichan++)
      plotarray[ichan+jchan] = pband[ichan];

    jchan += nchan;
  }

  //std::cerr << "min=" << min << " max=" << max << std::endl;
  float diff = max - min;
  min -= .1 * diff;
  max += .1 * diff;
  //std::cerr << "min=" << min << " max=" << max << std::endl;

  if (user_max)
    max = user_max;

  double freq = info->get_centre_frequency();
  double bw = info->get_bandwidth();
  
  double fmin = freq - bw/2;
  double fmax = freq + bw/2;
  
  cpgsci(1);
  cpgswin(fmin, fmax, 0, timespan);
  cpgbox("BCNST", 0.0, 0, "BCNST", 0.0, 0);
  cpglab("Frequency (MHz)", "Time (s)", "Dynamic Spectrum");
  
  float fstep = bw/nchan;
  float tstep = timespan/ntime;
  float tmin  = 0.0;
  float trf[6] = {fmin-0.5*fstep, fstep, 0.0,
		  tmin-0.5*tstep, 0.0, tstep};

  cpgimag(&plotarray[0], nchan, ntime, 1, nchan, 1, ntime, min, max, trf);

}

#endif
