/***************************************************************************
 *
 *   Copyright (C) 2008-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PlotOptions.h"
#include "pgutil.h"

#include <cpgplot.h>

using namespace std;

Pulsar::PlotOptions::PlotOptions ()
{
  plot_device = "?";

  x_npanel = y_npanel = 1;

  surface_width = 0.0;

  aspect_ratio = 0.0;

  width_pixels = height_pixels = 0;
}

//! Extra usage information implemented by derived classes
std::string Pulsar::PlotOptions::get_usage ()
{
  return 
    " -D device        plot to the specified device \n"
    " -g WxH           plot dimensions in pixels, width times height \n"
    " -N XxY           plot panels, width by height \n"
    " -r ratio         aspect ratio (height/width) \n"
    " -w width         plot surface width (in centimetres) \n";
}

//! Extra option flags implemented by derived classes
std::string Pulsar::PlotOptions::get_options ()
{
  return "D:N:g:r:w:";
}

//! Parse a non-standard command
bool Pulsar::PlotOptions::parse (char code, const std::string& arg)
{
  const char* carg = arg.c_str();

  char delimiter;

  switch (code)
  {
  case 'D':
    plot_device = carg;
    break;
    
  case 'g':
    if (sscanf (carg, "%u%c%u", &width_pixels, &delimiter, &height_pixels) != 3)
    {
      cerr << application->get_name()
	   << ": error parsing -g " << carg << endl;
      return false;
    }
    break;

  case 'N':
    if (sscanf( carg, "%u%c%u", &x_npanel, &delimiter, &y_npanel ) != 3)
    {
      cerr << application->get_name() 
	   << ": error parsing -N " << carg << endl;
      return false;
    }
    break;

  case 'r':
    if (sscanf( carg, "%f", &aspect_ratio ) != 1)
    {
      cerr << application->get_name() 
	   << ": error parsing -r " << carg << endl;
      return false;
    }
    break;

  case 'w':
    if (sscanf( carg, "%f", &surface_width ) != 1) {
      cerr << application->get_name() 
	   << ": error parsing -w " << carg << endl;
      return false;
    }
    break;
   
  default:
    return false;
  }
  
  return true;
}

//! Open the graphics device and configure it
void Pulsar::PlotOptions::setup ()
{
  // open the plot device
  if (cpgopen(plot_device.c_str()) < 0)
    throw Error (InvalidParam, "Pulsar::PlotOptions::setup",
		 "Could not open plot device");

  // set the size of the plot
  if (surface_width || aspect_ratio)
    pgplot::set_paper_size (surface_width, aspect_ratio);

  if (width_pixels && height_pixels)
    pgplot::set_dimensions (width_pixels, height_pixels);

  // prompt before plotting the next page
  cpgask(1);

  // break the surface up into multiple panels
  if (x_npanel > 1 || y_npanel > 1)
    cpgsubp (x_npanel, y_npanel);

  // set the viewport with room for titles
  cpgsvp (0.15,0.9, 0.15,0.9);
}

void Pulsar::PlotOptions::finalize ()
{
  cpgend();
}
