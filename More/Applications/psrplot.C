/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"

#include "Pulsar/PlotFactory.h"
#include "Pulsar/PlotLoop.h"
#include "Pulsar/Plot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Interpreter.h"

#include "TextInterface.h"
#include "strutil.h"

#include <cpgplot.h>

using namespace Pulsar;
using namespace std;

//! Pulsar Archive Zapping application
class psrplot: public Pulsar::Application
{
public:

  //! Default constructor
  psrplot ();

  //! Return usage information 
  std::string get_usage ();

  //! Return getopt options
  std::string get_options ();

  //! Parse a command line option
  bool parse (char code, const std::string& arg);

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  // -P help
  void help_plot_types ();

  // -C help
  void help_plot_options (const string& name);

  // -F help
  void help_frame_options (const string& name);

protected:

  // Available plots
  PlotFactory factory;

  // Plot classes to be used
  vector< Reference::To<Plot> > plots;

  // Options to be set
  vector<string> options;

  // Indeces over which to loop
  PlotLoop loop;

  // Allow plot classes to preprocess data before plotting
  bool preprocess;

  // Overlay plots from different files on top of eachother
  bool overlay_files;
};

int main (int argc, char** argv)
{
  psrplot program;
  return program.main (argc, argv);
}

psrplot::psrplot () : Pulsar::Application ("psrplot",
					   "pulsar plotting program")
{
  has_manual = true;
  version = "$Id: psrplot.C,v 1.29 2009/06/11 05:06:07 straten Exp $";

  // print angles in degrees
  Angle::default_type = Angle::Degrees;

  // Allow plot classes to preprocess data before plotting
  preprocess = true;

  // Do not overlay plots from different files on top of eachother
  overlay_files = false;

  add( new Pulsar::PlotOptions );

  Pulsar::StandardOptions* preprocessor = new Pulsar::StandardOptions;
  preprocessor->get_interpreter()->allow_infinite_frequency = true;

  add( preprocessor );
}

using namespace std;

std::string psrplot::get_options ()
{
  return "A:c:C:Fl:Op:Ps:x";
}

std::string psrplot::get_usage ()
{
  return
    " -P               Help: list available plot types \n"
    " -C plot          Help: list options specific to 'plot' \n"
    " -A plot          Help: list common options for 'plot'\n"
    "\n"
    " -p plot          plot type \n"
    " -c cfg[,cfg2...] plot options \n"
    " -s style         multiple plot options in 'style' file \n"
    "\n"
    " -x               disable default preprocessing \n"
    " -O               overlay plots \n"
    " -F               overlay plots from multiple files \n"
    "\n"
    " -l name=<range>  loop over the range of the named parameter \n";
}

// load the vector of options into the specified plot
void set_options (Pulsar::Plot* plot, const vector<string>& options);

// load the string of options into one of the plots
void specific_options (string optarg, vector< Reference::To<Plot> >& plots);

// load the style file into one of the plots
void specific_style (string optarg, vector< Reference::To<Plot> >& plots);

//! Parse a command line option
bool psrplot::parse (char code, const std::string& arg)
{
  switch (code)
  {
    case 'A': 
      help_frame_options (arg.c_str());
      exit (0);

    case 'c':
      if (arg[0] == ':')
	specific_options (arg, plots);
      else
	separate (arg, options, ",");
      break;
      
    case 'C': 
      help_plot_options (arg);
      exit (0);

    case 'F':
      overlay_files = true;
      break;

    case 'l':
      loop.add_index( new TextIndex(arg) );
      break;

    case 'O':
      loop.set_overlay( true );
      break;

    case 'p':
      plots.push_back( factory.construct(arg) );
      break;

    case 'P':
      help_plot_types ();
      exit (0);

    case 's':
      if (arg[0] == ':')
	specific_style (arg, plots);
      else
	loadlines (arg, options);
      break;

    case 'x':
      preprocess = false;
      break;

  default:
    return false;
  }

  return true;
}


void psrplot::setup ()
{
  if (plots.empty())
    throw Error (InvalidState, "psrplot",
		 "please choose at least one plot style");

  if (options.size())
  {
    if (verbose)
      cerr << "psrplot: parsing options" << endl;
    for (unsigned iplot=0; iplot < plots.size(); iplot++)
      set_options (plots[iplot], options);
  }
}

void psrplot::process (Pulsar::Archive* archive)
{
  if (verbose)
    cerr << "psrplot: plotting " << archive->get_filename() << endl;

  if( !overlay_files && loop.get_overlay() )
    cpgpage();

  Reference::To<Archive> toplot = archive;

  for (unsigned iplot=0; iplot < plots.size(); iplot++)
  {
    if (verbose)
      cerr << "psrplot: iplot=" << iplot << endl;

    if (plots.size() > 1)
      toplot = archive->clone();
    
    if (preprocess)
      plots[iplot]->preprocess (toplot);
    
    loop.set_Archive (toplot);
    loop.set_Plot (plots[iplot]);
    loop.plot();
  }
}

void psrplot::help_plot_types ()
{
  cout << "Available Plots:\n" << factory.help() << endl;
}

void help_options (TextInterface::Parser* tui)
{
  cout << tui->help(true) << endl;
}

void psrplot::help_plot_options (const string& name)
{
  Plot* plot = factory.construct(name);
  help_options( plot->get_interface() );
}

void psrplot::help_frame_options (const string& name)
{
  Plot* plot = factory.construct (name);
  help_options( plot->get_frame_interface() );
}

void set_options (Pulsar::Plot* plot, const vector<string>& options)
{
  for (unsigned j = 0; j < options.size(); j++)
  {
    try
    {
      plot->configure (options[j]);
    }
    catch (Error& error)
    {
      cerr << "psrplot: Invalid option '" << options[j] << "' " 
	   << error.get_message() << endl;
      exit (-1);
    }
  }
}

// parses index from arg and removes it from the string
unsigned get_index (string& arg, vector< Reference::To<Plot> >& plots)
{
  unsigned index = fromstring<unsigned> ( stringtok (arg, ":") );
  if (index >= plots.size())
  {
    cerr << "psrplot: invalid plot index = " << index
	 << " nplot=" << plots.size() << endl;
    exit(-1);
  }
  return index;
}

// load the string of options into one of the plots
void specific_options (string arg, vector< Reference::To<Plot> >& plots)
{
  // get the plot index
  unsigned index = get_index (arg, plots);

  // separate the options
  vector<string> options;
  separate (arg, options, ",");

  // set them for the specified plot
  set_options (plots[index], options);
}

// load the style file into one of the plots
void specific_style (string arg, vector< Reference::To<Plot> >& plots)
{
  // get the plot index
  unsigned index = get_index (arg, plots);

  // load the options
  vector<string> options;
  loadlines (arg, options);

  // set them for the specified plot
  set_options (plots[index], options);
}

