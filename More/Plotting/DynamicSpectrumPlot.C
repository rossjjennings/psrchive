/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicSpectrumPlot.h"
#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include <float.h>
#include <cpgplot.h>

using namespace std;

Pulsar::DynamicSpectrumPlot::DynamicSpectrumPlot ()
{
  stringstream ss;
  ss << pgplot::ColourMap::default_colour_map;
  pgplot::ColourMap::Name _name;
  ss >> _name;
  colour_map.set_name( _name );

  get_frame()->set_y_scale( y_scale = new FrequencyScale );
  get_frame()->set_x_scale( x_scale = new TimeScale );

  // default is to invert the axis tick marks
  get_frame()->get_x_axis()->set_opt ("BCINTS");

  get_frame()->get_y_axis()->set_opt ("BCINTS");
  //get_frame()->get_y_axis()->set_alternate (true);

  get_frame()->get_label_above()->set_offset (1.0);

  // ensure that no labels are printed inside the frame
  get_frame()->get_label_below()->set_all (PlotLabel::unset);

  ipol.set_integrate();

  method = 0;

  zero_check = true;
}

TextInterface::Parser* Pulsar::DynamicSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

//! srange is the subint range
std::pair<int,int> Pulsar::DynamicSpectrumPlot::get_srange() const 
{
  return const_cast<PlotFrame*>(get_frame())->get_x_scale()->get_index_range();
}

void Pulsar::DynamicSpectrumPlot::set_srange (const std::pair<int,int> &range)
{ 
  get_frame()->get_x_scale()->set_index_range (range);
}

std::pair<unsigned,unsigned> 
Pulsar::DynamicSpectrumPlot::get_subint_range (const Archive* data)
{
  unsigned nsub = data->get_nsubint();
  std::pair<unsigned,unsigned> range;
  get_frame()->get_x_scale()->get_indeces (nsub, range.first, range.second);
  return range;
}

std::pair<unsigned,unsigned> 
Pulsar::DynamicSpectrumPlot::get_chan_range (const Archive* data)
{
  unsigned nchan = data->get_nchan();
  std::pair<unsigned,unsigned> range;
  get_frame()->get_y_scale()->get_indeces (nchan, range.first, range.second);
  return range;
}


void Pulsar::DynamicSpectrumPlot::draw (const Archive* data)
{
  colour_map.apply ();

  float x_min, x_max;
  get_frame()->get_x_scale()->get_range (x_min, x_max);

  std::pair<unsigned,unsigned> srange = get_subint_range (data);
  unsigned nsub = srange.second - srange.first;

  float y_min, y_max;
  get_frame()->get_y_scale()->get_range (y_min, y_max);

  std::pair<unsigned,unsigned> crange = get_chan_range (data);
  unsigned nchan = crange.second - crange.first;

  if (verbose)
    cerr << "Pulsar::DynamicSpectrumPlot::draw chan min=" << crange.first 
         << " max=" << crange.second << endl;

  // Fill in data array
  float *plot_array = new float [nchan * nsub];
  get_plot_array(data, plot_array);

  // Determine data min/max
  float data_min = FLT_MAX;
  float data_max = -FLT_MAX;
  for (unsigned ii=0; ii<nchan*nsub; ii++)
  {
    // Assume any points exactly equal to 0 are zero-weighted
    if (zero_check && plot_array[ii]==0.0) continue; 
    if (plot_array[ii] < data_min) 
      data_min = plot_array[ii];
    if (plot_array[ii] > data_max) 
      data_max = plot_array[ii];
  }

  // XXX debug
  // cerr << "DynamicSpectrumPlot data min/max = (" << data_min 
  //  << "," << data_max << ")" << endl;

  // plot boundaries do not necessarily align with integer array boundaries
  float x_imin = 0;
  float x_imax = 0;
  get_frame()->get_x_scale()->get_axis_indeces (data->get_nsubint(), x_imin, x_imax);

  float x_res = (x_max-x_min)/(x_imax-x_imin);
  float x_ioff = -x_imin + unsigned(x_imin) - 0.5;

  float y_imin = 0;
  float y_imax = 0;
  get_frame()->get_y_scale()->get_axis_indeces (data->get_nchan(), y_imin, y_imax);

  float y_res = (y_max-y_min)/(y_imax-y_imin);
  float y_ioff = -y_imin + unsigned(y_imin) - 0.5;

  float trf[6] = { x_min + x_ioff*x_res, x_res, 0.0f,
		   y_min + y_ioff*y_res, 0.0f, y_res };

  cpgimag (plot_array, nsub, nchan,
	   1, nsub, 1, nchan, 
	   data_min, data_max, trf);

  delete [] plot_array;
}

