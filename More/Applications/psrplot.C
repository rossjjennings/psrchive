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

  //! Verify setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Detroy the plots (some destructors will plot accumulated results)
  void finalize ();

  // -P help
  void help_plot_types ();

  // -C help
  void help_plot_options (const string& name);

  // -F help
  void help_frame_options (const string& name);

  // -p add plot
  void add_plot (const string& name);

  // -c add plot options
  void add_plot_options (const string& name);

  // -s load plot options
  void load_plot_options (const string& name);

  // -l add loop index
  void add_loop_index (const string& name);

  // -lci set loop colour indeces
  void parse_loop_colours (const string& name);
  std::string loop_colours;

  //! Very verbose mode
  void set_very_verbose ()
  { 
    Application::set_very_verbose();
    Pulsar::Plot::verbose = true;
  }

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  // Available plots
  PlotFactory factory;

  friend class SpecificPreprocessingOptions;
  Reference::To<StandardOptions> preprocessor;

  // Plots
  vector< Reference::To<Plot> > plots;

  // Options to be set
  vector<string> options;

  // Indeces over which to loop
  PlotLoop loop;

  // Allow plot classes to preprocess data before plotting
  bool preprocess;

  // Overlay plots from different files on top of eachother
  bool overlay_files;

  // Overlay plots on top of eachother
  bool overlay_plots;

  // Stack multiple plots vertically
  bool stack_plots;
};

int main (int argc, char** argv)
{
  psrplot program;
  return program.main (argc, argv);
}


//! Extends StandardOptions to enable plot-specific pre-processing
class SpecificPreprocessingOptions : public Pulsar::StandardOptions
{
public:

  //! For now, works only with the psrplot class
  SpecificPreprocessingOptions (psrplot*);

  //! Extends StandardOptions::add_job
  void add_job (const std::string& job);
  
  //! Extends StandardOptions::add_script
  void add_script (const std::string& job);

  //! Adds to the pre-processing options for a specific plot
  void specific_job (std::string job);
  
  //! Adds to the pre-processing options for a specific plot
  void specific_script (std::string job);

  //! Provide access to custom interpreter
  Interpreter* get_interpreter ();

private:

  Reference::To<psrplot> program;

};

SpecificPreprocessingOptions::SpecificPreprocessingOptions (psrplot* p)
{
  program = p;
}

Pulsar::Interpreter* SpecificPreprocessingOptions::get_interpreter ()
{
  Pulsar::Interpreter* shell = StandardOptions::get_interpreter ();
  shell->allow_infinite_frequency = true;
  return shell;
}

void SpecificPreprocessingOptions::add_job (const std::string& arg)
{
  if (arg[0] == ':')
    specific_job (arg);
  else
    StandardOptions::add_job (arg);
}
  
void SpecificPreprocessingOptions::add_script (const std::string& arg)
{
  if (arg[0] == ':')
    specific_script (arg);
  else
    StandardOptions::add_script (arg);
}

psrplot::psrplot () : Pulsar::Application ("psrplot",
					   "pulsar plotting program")
{
  has_manual = true;
  version = "$Id: psrplot.C,v 1.35 2010/09/19 08:30:00 straten Exp $";

  // Allow plot classes to preprocess data before plotting
  preprocess = true;

  // Do not overlay plots from different files on top of eachother
  overlay_files = false;

  // Do not overlay plots
  overlay_plots = false;

  // Do not stack multiple plots
  stack_plots = false;

  add( new Pulsar::PlotOptions );
  add( preprocessor = new SpecificPreprocessingOptions(this) );
}

void psrplot::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psrplot::help_plot_types, 'P');
  arg->set_help ("Help: list available plot types");

  arg = menu.add (this, &psrplot::help_plot_options, 'C', "plot");
  arg->set_help ("Help: list options specific to 'plot'");

  arg = menu.add (this, &psrplot::help_frame_options, 'A', "plot");
  arg->set_help ("Help: list common options for 'plot'");

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psrplot::add_plot, 'p', "plot");
  arg->set_help ("add plot type");

  arg = menu.add (this, &psrplot::add_plot_options, 'c', "cfg[s]");
  arg->set_help ("add plot options");

  arg = menu.add (this, &psrplot::load_plot_options, 's', "style");
  arg->set_help ("add plot options in 'style' file");

  // blank line in help
  menu.add ("");

  arg = menu.add (preprocess, 'x');
  arg->set_help ("disable default preprocessing");

  arg = menu.add (stack_plots, 'S');
  arg->set_help ("stack plots");

  arg = menu.add (overlay_plots, 'O');
  arg->set_help ("overlay plots");

  arg = menu.add (overlay_files, 'F');
  arg->set_help ("overlay plots from multiple files");

  // blank line in help
  menu.add ("");

  arg = menu.add (this, &psrplot::add_loop_index, 'l', "name=<range>");
  arg->set_help ("loop over the range of the named parameter");

  arg = menu.add (loop_colours, "lci", "<range>");
  arg->set_help ("set the colour indeces to loop over");
}

void psrplot::add_plot (const string& name)
{
  plots.push_back( factory.construct(name) );
  loop.add_Plot (plots.back());
}

// load the string of options into one of the plots
void specific_options (string optarg, vector< Reference::To<Plot> >& plots);

void psrplot::add_plot_options (const std::string& arg)
{
  if (arg[0] == ':')
    specific_options (arg, plots);
  else
    separate (arg, options, ",");
}
      
// load the style file into one of the plots
void specific_style (string optarg, vector< Reference::To<Plot> >& plots);

// set the option for the specified plot
void set_option (Pulsar::Plot* plot, const string& option);

// get the preprocessor for the specified plot
Pulsar::StandardOptions* get_preprocessor (Plot* plot);

void psrplot::load_plot_options (const std::string& arg)
{
  if (arg[0] == ':')
  {
    specific_style (arg, plots);
    return;
  }

  vector<string> style;
  loadlines (arg, style);

  Reference::To<Plot> current_plot;

  for (unsigned i=0; i<style.size(); i++)
  {
    string line = style[i];

    string key = stringtok (line, " \t\n");

    if (key == "plot")
    {
      // the rest of the line is the new plot to add
      add_plot (line);
      current_plot = plots.back();
    }
    else if (key == "exec")
    {
      // the rest of the line is the psrsh pre-processing command to run
      if (current_plot)
	get_preprocessor(current_plot)->add_job(line);
      else
	preprocessor->add_job(line);
    }
    else
    {
      // the entire line is a plot configuration option
      if (current_plot)
	set_option( current_plot, style[i] );
      else
	options.push_back( style[i] );
    }
  }
}

void psrplot::add_loop_index (const std::string& arg)
{
  loop.add_index( new TextIndex(arg) );
}

void psrplot::parse_loop_colours (const string& txt)
{
  vector<unsigned> ci;

  int min, max;
  cpgqcol (&min, &max);

  // cerr << "psrplot::parse_loop_colours min=" << min << " max=" << max << endl;

  TextInterface::parse_indeces (ci, txt, max);

  // cerr << "psrplot::parse_loop_colours size=" << ci.size() << endl;

  loop.set_colour_indeces (ci);
}

// load the vector of options into the specified plot
void set_options (Pulsar::Plot* plot, const vector<string>& options);

void psrplot::setup ()
{
  if (plots.empty())
    throw Error (InvalidState, "psrplot",
		 "please choose at least one plot style");

  // -F should activate -O automatically:
  if (overlay_files)
    overlay_plots = true;

  loop.set_stack (stack_plots);
  loop.set_overlay (overlay_plots);
  loop.set_preprocess (preprocess);
  loop.configure (options);

  if (!loop_colours.empty())
    parse_loop_colours (loop_colours);
}

void psrplot::process (Pulsar::Archive* archive)
{
  if (verbose)
    cerr << "psrplot: plotting " << archive->get_filename() << endl;

  if( !overlay_files && loop.get_overlay() )
    cpgpage();

  loop.set_Archive (archive);
  loop.plot();

  if( !overlay_files )
    loop.finalize();
}

//! Final steps, run once at end of program
void psrplot::finalize ()
{
  loop.finalize ();
}

void psrplot::help_plot_types ()
{
  cout << "Available Plots:\n" << factory.help() << endl;
  exit (0);
}

void help_options (TextInterface::Parser* tui)
{
  cout << tui->help(true) << endl;
  exit (0);
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

void set_option (Pulsar::Plot* plot, const string& option) try
{
  plot->configure (option);
}
catch (Error& error)
{
  cerr << "psrplot: Invalid option '" << option << "' " 
       << error.get_message() << endl;
  exit (-1);
}

void set_options (Pulsar::Plot* plot, const vector<string>& options)
{
  for (unsigned j = 0; j < options.size(); j++)
    set_option (plot, options[j]);
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

// get the pre-processor for the given plot
Pulsar::StandardOptions* get_preprocessor (Plot* plot)
{
  Pulsar::StandardOptions* options = 0;

  if (plot->has_preprocessor())
    options = dynamic_cast<Pulsar::StandardOptions*>(plot->get_preprocessor());

  if (!options)
  {
    options = new Pulsar::StandardOptions;
    plot->set_preprocessor (options);
  }

  return options;
}

// load the string of pre-processor commands into one of the plots
void SpecificPreprocessingOptions::specific_job (string arg)
{
  unsigned index = get_index (arg, program->plots);
  get_preprocessor(program->plots[index])->add_job(arg);
}

// load the pre-processor script into one of the plots
void SpecificPreprocessingOptions::specific_script (string arg)
{
  unsigned index = get_index (arg, program->plots);
  get_preprocessor(program->plots[index])->add_script(arg);
}

