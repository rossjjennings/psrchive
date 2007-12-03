/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/AnglePlot.h"
#include "Pulsar/AnglePlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "templates.h"

#include <cpgplot.h>
#include <algorithm>


using namespace std;

Pulsar::AnglePlot::AnglePlot()
{
  marker = ErrorBar;
  threshold = 4.0;
  span = 0.0;

  isubint = 0;
  ichan = 0;

  get_frame()->get_y_axis()->set_tick( 60.0 );
  get_frame()->get_y_axis()->set_nsub( 3 );
}

TextInterface::Parser* Pulsar::AnglePlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::AnglePlot::prepare (const Archive* data)
{
  get_angles (data);

  if (angles.size() != data->get_nbin())
    throw Error (InvalidState, "Pulsar::AnglePlot::prepare",
                 "angles vector size=%u != nbin=%u",
                 angles.size(), data->get_nbin());

  float min = 0;
  float max = 0;

  // TODO:DS The problem here is there is no way to know if the user has
  //         set a range of 0,1 on the command line, which is possibe
  //         have to look into refining the "preferences" system.

  pair<float,float> range = get_frame()->get_y_scale()->get_range_norm();
  if( range.first == 0 && range.second == 1 )
  {
    if (span)
    {
      // keep pgplot from drawing the 90 or 180 at the edge
      float half = 0.5 * span - 0.0001;
      min = -half;
      max = half;
    }
    else
    {
      unsigned i_min, i_max;
      get_scale()->get_indeces (data, i_min, i_max);
      Estimate<double> e_min, e_max;
      cyclic_minmax (angles, i_min, i_max, e_min, e_max);
      min = e_min.get_value();
      max = e_max.get_value();
    }
  }
  else
  {
    min = -90;
    max = 90;
  }

  cerr << "setting minmax to (" << min << "," << max << ")" << endl;
  get_frame()->get_y_scale()->set_minmax ( min, max );
}

void Pulsar::AnglePlot::draw (const Archive *data)
{
  std::vector<float> phases;
  get_scale()->get_ordinates (data, phases);

  pair<float,float> x_range = get_scale()->get_range_norm();

  int range_start = int(floor(x_range.first));
  int range_end = int(ceil(x_range.second));

  pair<float,float> y_range = get_frame()->get_y_scale()->get_range_norm();

  cerr << "y range actual is (" << y_range.first << "," << y_range.second << ")" << endl;
  
  int y_range_start = int(floor(y_range.first));
  int y_range_end = int(ceil(y_range.second));
  
  cerr << "y range ints are (" << y_range_start << "," << y_range_end << ")" << endl;

  for( int range = range_start; range < range_end; range ++ )
  {
    //     float yoff = 0;
    //     unsigned times = 1;

    float xoff = float(range) * get_scale()->get_scale(data);

    //     if (span)
    //     {
    //       yoff = -span;
    //       times = 3;
    //     }

    bool err1 = false;
    float terminal = 0.0;

    if (marker & ErrorBar)
    {
      terminal = 1.0;
      err1 = true;
    }

    if (marker & ErrorTick)
    {
      terminal = 0.0;
      err1 = true;
    }

    float old_ch;
    cpgqch( &old_ch );
    cpgsch( 0.5 );
//     for (unsigned ioff=0; ioff < times; ioff++)
//     {
      for (unsigned ibin=0; ibin < phases.size(); ibin++)
        if (angles[ibin].get_variance() != 0)
        {
          for( int yoff = y_range_start; yoff <= y_range_end; yoff ++ )
          {
            if (err1)
              cpgerr1 (6, phases[ibin]+xoff, angles[ibin].get_value() + yoff*180,
                       angles[ibin].get_error(), terminal);
            if (marker & Dot)
            {
              cpgpt1 (phases[ibin]+xoff, angles[ibin].get_value() + yoff*180, 17);
            }
          }
        }

      //       yoff += span;
//     }
    cpgsch( old_ch );
  }

}

//! Return the label for the y-axis
std::string Pulsar::AnglePlot::get_flux_label (const Archive* data)
{
  return "P.A. (deg.)";
}

std::ostream& Pulsar::operator << (std::ostream& os, AnglePlot::Marker marker)
{
  if (marker & AnglePlot::Dot)
  {
    os << "dot";
    if (marker & ~AnglePlot::Dot)
      os << "+";
  }

  if (marker & AnglePlot::ErrorBar)
    os << "bar";

  if (marker & AnglePlot::ErrorTick)
    os << "tick";

  return os;
}

std::istream& Pulsar::operator >> (std::istream& is, AnglePlot::Marker& marker)
{
  std::string input;
  is >> input;

  marker = (AnglePlot::Marker) 0;

  if (input == "dot")
  {
    marker = AnglePlot::Dot;
    return is;
  }

  if (input.substr(0,4) == "dot+")
  {
    marker = AnglePlot::Dot;
    input.erase(0,4);
  }

  if (input == "bar")
    marker = (AnglePlot::Marker) (marker | AnglePlot::ErrorBar);

  else if (input == "tick")
    marker = (AnglePlot::Marker) (marker | AnglePlot::ErrorTick);

  else
    throw Error (InvalidParam, "operator >> AnglePlot::Marker",
                 "invalid code = " + input);

  return is;
}


