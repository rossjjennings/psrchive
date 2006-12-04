/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
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

  above = new PlotLabel;
  below = new PlotLabel;

  get_label_above()->set_centre("$file");
  get_label_below()->set_left("$name.$freq MHz");
  get_label_below()->set_spacing( -get_label_below()->get_spacing() );

  transpose = false;
}

Pulsar::PlotFrame::~PlotFrame ()
{
}

#include "iopair.h"

void Pulsar::PlotFrame::focus (const Archive* data)
{
  std::pair<float,float> xvp = get_x_scale(true)->get_viewport ();
  std::pair<float,float> yvp = get_y_scale(true)->get_viewport ();

  cpgsvp (xvp.first, xvp.second, yvp.first, yvp.second);

  float x_min, x_max;
  get_x_scale(true)->get_range (data, x_min, x_max);

  float y_min, y_max;
  get_y_scale(true)->get_range (data, y_min, y_max);

  if (Plot::verbose)
    cerr << "Pulsar::PlotFrame::focus xmin=" << x_min << " xmax=" << x_max
	 << " ymin=" << y_min << " ymax=" << y_max << endl;

  cpgswin (x_min, x_max, y_min, y_max);

  cpgsch (character_height);
  cpgscf (character_font);
  cpgslw (line_width);
}

void Pulsar::PlotFrame::draw_axes (const Archive* data)
{
  cpgsls (1);
  cpgsci (1);

  if (Plot::verbose)
    cerr << "Pulsar::PlotFrame::draw_axes"
      " xopt='" << get_x_axis(true)->get_pgbox_opt() << "'"
      " yopt='" << get_y_axis(true)->get_pgbox_opt() << "'" << endl;

  cpgbox( get_x_axis(true)->get_pgbox_opt().c_str(), 0.0, 0,
	  get_y_axis(true)->get_pgbox_opt().c_str(), 0.0, 0 );
}

void Pulsar::PlotFrame::label_axes (const string& default_x,
				    const string& default_y)
{
  cpgsls (1);
  cpgsci (1);

  string xlabel = get_x_axis()->get_label();
  if (xlabel == PlotLabel::unset)
    xlabel = default_x;

  string ylabel = get_y_axis()->get_label();
  if (ylabel == PlotLabel::unset)
    ylabel = default_y;

  if (transpose)
    swap (xlabel, ylabel);

  cpgmtxt ("L",2.5,.5,.5, ylabel.c_str());
  cpgmtxt ("B",2.5,.5,.5, xlabel.c_str());
}

void Pulsar::PlotFrame::decorate (const Archive* data)
{
  cpgsls (1);
  cpgsci (1);

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

void Pulsar::PlotFrame::set_label_above (PlotLabel* label)
{
  above = label;
}

void Pulsar::PlotFrame::set_label_below (PlotLabel* label)
{
  below = label;
}

void Pulsar::PlotFrame::set_publication_quality (bool flag)
{
  PlotAttributes::set_publication_quality (flag);

  if (flag)
    get_label_above()->set_centre(PlotLabel::unset);
  else
    get_label_above()->set_centre("$file");
}

void Pulsar::PlotFrame::set_plot_labels (bool flag)
{
  PlotAttributes::set_plot_labels (flag);
 
  if (!flag) {
    get_x_axis()->rem_pgbox_opt('N');
    get_y_axis()->rem_pgbox_opt('N');

    get_x_axis()->set_label(" ");
    get_y_axis()->set_label(" ");

    get_label_above()->set_all(PlotLabel::unset);
    get_label_below()->set_all(PlotLabel::unset);
  }
}

//! Set the viewport (normalized device coordinates)
void Pulsar::PlotFrame::set_viewport (float x0, float x1,
				      float y0, float y1)
{
  get_x_scale()->set_viewport( std::pair<float,float> (x0,x1) );
  get_y_scale()->set_viewport( std::pair<float,float> (y0,y1) );
}
