/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicSpectrum.h"
#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <float.h>
#include <cpgplot.h>

using namespace std;

Pulsar::DynamicSpectrum::DynamicSpectrum ()
{
  colour_map.set_name( pgplot::ColourMap::Heat );

  get_frame()->set_y_scale( new FrequencyScale );

  // default is to invert the axis tick marks
  get_frame()->get_x_axis()->set_opt ("BCINTS");

  get_frame()->get_y_axis()->set_opt ("BCINTS");
  //get_frame()->get_y_axis()->set_alternate (true);

  get_frame()->get_label_above()->set_offset (1.0);

  // ensure that no labels are printed inside the frame
  get_frame()->get_label_below()->set_all (PlotLabel::unset);

  pol = 0;

  srange.first = -1;
  srange.second = -1;

  crange.first = -1;
  crange.second = -1;

}

TextInterface::Parser* Pulsar::DynamicSpectrum::get_interface ()
{
  return new Interface (this);
}

void Pulsar::DynamicSpectrum::preprocess (const Archive* data)
{
}

void Pulsar::DynamicSpectrum::prepare (const Archive* data)
{

  // Figure out subint range
  // TODO make this axis be time...
  // TODO also, move this to draw()?
  if (srange.first<0) {
    srange.first = 0;
  }
  if (srange.second>data->get_nsubint()-1 || srange.second<0) {
    srange.second = data->get_nsubint() - 1;
  }
  get_frame()->get_x_scale()->set_minmax(srange.first, srange.second+1);

}

void Pulsar::DynamicSpectrum::draw (const Archive* data)
{
  colour_map.apply ();

  // Guess we need to do this range stuff here rather than in prepare()..
  unsigned min_chan, max_chan;
  get_frame()->get_y_scale()->get_indeces (data->get_nchan(), 
      min_chan, max_chan);
  if (max_chan >= data->get_nchan())
    max_chan = data->get_nchan() - 1;
  crange.first = min_chan;
  crange.second = max_chan;

  // Dimensions
  int nchan = crange.second - crange.first + 1;
  int nsub = srange.second - srange.first + 1;

  // Fill in data array
  float *plot_array = new float [nchan * nsub];
  get_plot_array(data, plot_array);

  // Determine data min/max
  float data_min = FLT_MAX;
  float data_max = FLT_MIN;
  for (int ii=0; ii<nchan*nsub; ii++) {
    // Assume any points exactly equal to 0 are zero-weighted
    if (plot_array[ii]==0.0) continue; 
    if (plot_array[ii] < data_min) 
      data_min = plot_array[ii];
    if (plot_array[ii] > data_max) 
      data_max = plot_array[ii];
  }

  cpgswin(srange.first, srange.second+1, crange.first, crange.second+1);
  float trans[6] = {srange.first-0.5, 1.0, 0.0, crange.first, 0.0, 1.0};
  cpgimag(plot_array, nsub, nchan, 1, nsub, 1, nchan, 
      data_min, data_max, trans);

  delete [] plot_array;

}

std::string Pulsar::DynamicSpectrum::get_xlabel (const Archive *data)
{
  return "Subintegration";
}

std::string Pulsar::DynamicSpectrum::get_ylabel (const Archive *data)
{
  return "Frequency (MHz)";
}
