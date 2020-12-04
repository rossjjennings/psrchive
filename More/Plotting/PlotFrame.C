/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotFrame.h"
#include "Pulsar/Plot.h"

#include <cpgplot.h>

using namespace std;

Pulsar::PlotFrame::PlotFrame ()
{
  x_scale = new PlotScale;
  x_axis = new PlotAxis;

  y_scale = new PlotScale;
  y_axis = new PlotAxis;

  character_height = 1.0;
  character_font = 1;
  line_width = 1;

  below = new PlotLabel;

  get_label_below()->set_left("$name\n$freq MHz");
  get_label_below()->set_spacing( -get_label_below()->get_spacing() );

  transpose = false;
  
  draw_box = true;

  frozen = false;
}

Pulsar::PlotFrame::~PlotFrame ()
{
}

#include "pairutil.h"

void Pulsar::PlotFrame::init (const Archive* data)
{
  if (frozen)
    return;

  get_x_scale(true)->init (data);
  get_y_scale(true)->init (data);
}

void Pulsar::PlotFrame::focus (const Archive* data)
{
  init (data);

  std::pair<float,float> xvp = get_x_scale(true)->get_viewport ();
  std::pair<float,float> yvp = get_y_scale(true)->get_viewport ();

  if (Plot::verbose)
    cerr << "Pulsar::PlotFrame::focus cpgsvp (" 
	 << xvp.first << ", " << xvp.second << ", " 
	 << yvp.first << ", " << yvp.second << ")" << endl;

  cpgsvp (xvp.first, xvp.second, yvp.first, yvp.second);

  float x_min, x_max;
  get_x_scale(true)->get_range (x_min, x_max);

  float y_min, y_max;
  get_y_scale(true)->get_range (y_min, y_max);

  if (Plot::verbose)
    cerr << "Pulsar::PlotFrame::focus cpgswin ("
	 << x_min << ", " << x_max << ", "
	 << y_min << ", " << y_max << ")" << endl;

  cpgswin (x_min, x_max, y_min, y_max);

  setup ();
}

void Pulsar::PlotFrame::draw_axes (const Archive* data)
{
  setup ();

  float x_min, x_max;
  get_x_scale(true)->get_range_external (x_min, x_max);

  float y_min, y_max;
  get_y_scale(true)->get_range_external (y_min, y_max);

  if (Plot::verbose)
    cerr << "Pulsar::PlotFrame::draw_axes cpgswin ("
	 << x_min << ", " << x_max << ", "
	 << y_min << ", " << y_max << ")" << endl;

  cpgswin (x_min, x_max, y_min, y_max);

  PlotAxis* xAxis = get_x_axis(true);
  PlotAxis* yAxis = get_y_axis(true);

  if( draw_box )
  {
    if (Plot::verbose)
      cerr << "Pulsar::PlotFrame::draw_axes cpgbox (" 
	   << xAxis->get_opt().c_str() << ", " 
	   << xAxis->get_tick() << ", " << xAxis->get_nsub() << ", "
	   << yAxis->get_opt().c_str() << ", "
	   << yAxis->get_tick() << ", " << yAxis->get_nsub() << ")" << endl;

    cpgbox( xAxis->get_opt().c_str(),
	    xAxis->get_tick(),
	    xAxis->get_nsub(),
	    yAxis->get_opt().c_str(),
	    yAxis->get_tick(),
	    yAxis->get_nsub() );
  }
}

bool unset (const string& txt)
{
  return txt.empty() || txt == "" || txt == Pulsar::PlotLabel::unset;
}

void Pulsar::PlotFrame::label_axes (const string& default_x,
				    const string& default_y)
{
  setup ();

  /*
    Axis label priority:
    1) get_?_axis()->get_label() - set by user, overrides all
    2) default_? - set by calling object, overrides scale default
    3) get_?_scale()->get_label() - set by the scale object (default)
  */

#ifdef _DEBUG
  cerr << "Pulsar::PlotFrame::label_axes"
    " x_axis::label='" << get_x_axis()->get_label() << "'" << endl;

  cerr << "Pulsar::PlotFrame::label_axes"
    " default_x='" << default_x << "'" << endl;
    
  cerr << "Pulsar::PlotFrame::label_axes"
    " x_scale::label='" << get_x_scale()->get_label() << "'" << endl;
#endif

  string xlabel = get_x_axis()->get_label();
  if ( unset(xlabel) )
    xlabel = default_x;
  if ( unset(xlabel) )
    xlabel = get_x_scale()->get_label();

  string ylabel = get_y_axis()->get_label();
  if ( unset(ylabel) )
    ylabel = default_y;    
  if ( unset(ylabel) )
    ylabel = get_y_scale()->get_label();

  if (transpose)
    swap (xlabel, ylabel);

#ifdef _DEBUG
  cerr << "Pulsar::PlotFrame::label_axes "
    "x='" << xlabel << "' y='" << ylabel << "'" << endl;
#endif

  if (xlabel.empty())
    xlabel = " ";

  if (ylabel.empty())
    ylabel = " ";

  cpgmtxt ("L",get_y_axis()->get_displacement(),.5,.5, ylabel.c_str());
  cpgmtxt ("B",get_x_axis()->get_displacement(),.5,.5, xlabel.c_str());
}

void Pulsar::PlotFrame::decorate (const Archive* data)
{
  setup ();

  get_label_above()->plot(data);
  get_label_below()->plot(data);
}

//! Get the x-scale
Pulsar::PlotScale* Pulsar::PlotFrame::get_x_scale (bool allow_transpose)
{ 
  if (allow_transpose && transpose)
    return y_scale;
  else
    return x_scale;
}

//! Set the x-scale
void Pulsar::PlotFrame::set_x_scale (PlotScale* scale)
{
  x_scale = scale;
}

//! Get the x-axis
Pulsar::PlotAxis* Pulsar::PlotFrame::get_x_axis (bool allow_transpose)
{
  if (allow_transpose && transpose)
    return y_axis;
  else
    return x_axis;
}

//! Get the x-scale
Pulsar::PlotScale* Pulsar::PlotFrame::get_y_scale (bool allow_transpose)
{ 
  if (allow_transpose && transpose)
    return x_scale;
  else
    return y_scale;
}

//! Set the y-scale
void Pulsar::PlotFrame::set_y_scale (PlotScale* scale)
{
  y_scale = scale;
}

//! Get the x-axis
Pulsar::PlotAxis* Pulsar::PlotFrame::get_y_axis(bool allow_transpose)
{
  if (allow_transpose && transpose)
    return x_axis;
  else
    return y_axis;
}

void Pulsar::PlotFrame::set_label_below (PlotLabel* label)
{
  below = label;
}

void Pulsar::PlotFrame::no_labels ()
{
  PlotAttributes::no_labels ();
 
  get_x_axis()->rem_opt('N');
  get_y_axis()->rem_opt('N');
  
  get_x_axis()->set_label(" ");
  get_y_axis()->set_label(" ");
  
  get_label_above()->set_all(PlotLabel::unset);
  get_label_below()->set_all(PlotLabel::unset);
}

//! Set the viewport (normalized device coordinates)
void Pulsar::PlotFrame::set_viewport (float x0, float x1,
				      float y0, float y1)
{
  get_x_scale()->set_viewport( std::pair<float,float> (x0,x1) );
  get_y_scale()->set_viewport( std::pair<float,float> (y0,y1) );
}

void Pulsar::PlotFrame::include (PlotFrame* frame)
{
  get_x_scale()->include( frame->get_x_scale() );
  get_y_scale()->include( frame->get_y_scale() );
}

void Pulsar::PlotFrame::copy (PlotFrame* frame)
{
  get_x_scale()->copy( frame->get_x_scale() );
  get_y_scale()->copy( frame->get_y_scale() );
}

 //! Freeze the frame
void Pulsar::PlotFrame::freeze (bool f)
{
  frozen = f;
  get_x_scale()->freeze(f);
  get_y_scale()->freeze(f);
}

