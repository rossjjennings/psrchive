/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#include "Pulsar/FreqVsSNRPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include <cpgplot.h>
#include <float.h>
#include <algorithm>
#include <Pulsar/FrequencyScale.h>



using namespace Pulsar;
using namespace std;



FreqVsSNRPlot::FreqVsSNRPlot()
{
  subint = 0;
  pol = 0;
  //get_frame()->set_x_scale( new FrequencyScale );
}



void FreqVsSNRPlot::prepare( const Archive *data )
{
}



void FreqVsSNRPlot::draw( const Archive *data )
{
  bool svp = true;
  
  int nchan = data -> get_nchan();

  vector <float> snrs(nchan);



  for (unsigned i = 0; i < data->get_nchan(); i++)
  {
    snrs[i] = data->get_Profile(subint, pol, i)->snr();
  }

  float snr_min = FLT_MAX;
  float snr_max = FLT_MIN;
  
  snr_min = min( snr_min, *std::min_element (snrs.begin(), snrs.end() ) );
  
  snr_max = max( snr_max, *std::max_element (snrs.begin(), snrs.end() ) );

  if (svp)
    cpgsvp(0.1, 0.9, 0.1, 0.9);
  cpgswin (0, data->get_nchan(), snr_min - (snr_min/10), snr_max + (snr_max/10));
  cpgslw(1);
  cpgsch(1.0);
  cpgsci(1);

  cpgsch(1);

  for (unsigned k = 0; k < data->get_nchan(); k++)
  {
    cpgsci(3);
    cpgpt1(k, snrs[k], 0);
  }
  
  int min_chan = 0;
  int max_chan = data->get_nbin();
  
  Reference::To<PlotScale> sy = get_frame()->get_y_scale();
  sy->set_minmax( snr_min, snr_max );
  
  Reference::To<PlotScale> sx = get_frame()->get_x_scale();
  sx->set_minmax( min_chan, max_chan );
}


string FreqVsSNRPlot::get_xlabel( const Archive *data )
{
  return "Channel";
}



string FreqVsSNRPlot::get_ylabel( const Archive *data ) 
{
  return "SNR";
}



TextInterface::Class *FreqVsSNRPlot::get_interface( void )
{
  return new Interface( this );
}


