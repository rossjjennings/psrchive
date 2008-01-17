/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/PlotOptions.h"

#include <cpgplot.h>

Pulsar::PlotOptions::PlotOptions ()
{
  plot_device = "?";
  x_npanel = 1;
  y_npanel = 1;
}

//! Extra usage information implemented by derived classes
std::string Pulsar::PlotOptions::get_usage ()
{
  return 
    " -D device        plot to the specified device \n"
    " -N x,y           divide the plot surface into x by y panels \n";
}

//! Extra option flags implemented by derived classes
std::string Pulsar::PlotOptions::get_options ()
{
  return "D:N:";
}

//! Parse a non-standard command
bool Pulsar::PlotOptions::parse (char code, const std::string& arg)
{
  switch (code)
  {
  case 'D':
    plot_device = optarg;
    break;
    
  case 'N':
    {
      char delimiter;
      if (sscanf( optarg, "%d%c%d", &x_npanel, &delimiter, &y_npanel ) != 2)
      {
	cerr << application->get_name() 
	     << ": error parsing -N " << arg << endl;
	return false;
      }
      break;
      
    default:
      return false;
    }
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

  // prompt before plotting the next page
  cpgask(1);

  if (x_npanel > 1 || y_npanel > 1)
    cpgsubp(x_npanel, y_npanel);
}
