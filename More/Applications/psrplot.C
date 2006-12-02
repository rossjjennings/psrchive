/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotFactory.h"
#include "Pulsar/Plot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Interpreter.h"

#include "TextInterface.h"
#include "strutil.h"
#include "dirutil.h"

#include <cpgplot.h>

#include <unistd.h>

using namespace Pulsar;
using namespace std;

// Available plots
static PlotFactory factory;

void usage ()
{
  cerr << 
    "psrplot - pulsar plotting program \n"
    "\n"
    "psrplot [options] filename[s]\n"
    "options:\n"
    "\n"
    " -P               Help: list available plot types \n"
    " -C plot          Help: list options specific to 'plot' \n"
    " -A plot          Help: list common options for 'plot'\n"
    "\n"
    " -D device        plot device \n"
    " -N x,y           plot panels \n"
    " -O               overlay plots \n"
    "\n"
    " -p plot          plot type \n"
    " -c cfg[,cfg2...] plot options \n"
    " -s style         multiple plot options in 'style' file \n"
    "\n"
    " -j job[,job2...] preprocessing jobs \n"
    " -J jobs          multiple preprocessing jobs in 'jobs' file \n"
    "\n"
    " -h               This help page \n"
    " -M metafile      Specify list of archive filenames in metafile \n"
    " -q               Quiet mode \n"
    " -v               Verbose mode \n"
    " -V               Very verbose mode \n"
    "\n"
       << endl;
}

// -P help
void help_plot_types ();

// -C help
void help_plot_options (const char* name);

// -F help
void help_frame_options (const char* name);

// load the vector of options into the specified plot
void set_options (Pulsar::Plot* plot, const vector<string>& options);

// load the string of options into one of the plots
void specific_options (string optarg, vector<Plot*>& plots);

// load the style file into one of the plots
void specific_style (string optarg, vector<Plot*>& plots);

int main (int argc, char** argv) try {

  // name of file containing list of Archive filenames
  char* metafile = NULL;

  // PGPLOT device name
  string plot_device = "?";

  // Plot classes to be used
  vector<Plot*> plots;

  // Options to be set
  vector<string> options;

  // Preprocessing jobs
  vector<string> jobs;

  // Overlay plots (do not clear page between different plot types)
  bool overlay = false;

  // verbosity
  bool verbose = false;

  int n1 = 1;
  int n2 = 1;

  static char* args = "A:c:C:D:hj:J:M:N:Op:Pqs:vV";

  char c = 0;
  while ((c = getopt (argc, argv, args)) != -1) 

    switch (c)  {

    case 'p':
      plots.push_back( factory.construct(optarg) );
      break;

    case 'P':
      help_plot_types ();
      return 0;

    case 'c':
      if (optarg[0] == ':')
	specific_options (optarg, plots);
      else
	separate (optarg, options, ",");
      break;
      
    case 'C': 
      help_plot_options (optarg);
      return 0;

    case 's':
      if (optarg[0] == ':')
	specific_style (optarg, plots);
      else
	loadlines (optarg, options);
      break;

    case 'D':
      plot_device = optarg;
      break;

    case 'A': 
      help_frame_options (optarg);
      return 0;

    case 'h':
      usage();
      return 0;

    case 'j':
      separate (optarg, jobs, ",");
      break;
      
    case 'J':
      loadlines (optarg, jobs);
      break;

    case 'M':
      metafile = optarg;
      break;

    case 'N':
      if (sscanf( optarg, "%d,%d", &n1, &n2 ) != 2) {
	cerr << "psrplot: error parsing -N " << optarg << endl;
	return -1;
      }

    case 'O':
      overlay = true;
      break;

    case 'q':
      Archive::set_verbosity (0);
      break;

    case 'v':
      Archive::set_verbosity (2);
      verbose = true;
      break;

    case 'V':
      Archive::set_verbosity (3);
      Plot::verbose = true;
      verbose = true;
      break;

   } 


  if (plots.empty()) {
    cout << "psrplot: please choose at least one plot style" << endl;
    return -1;
  } 

  if (options.size())
    for (unsigned iplot=0; iplot < plots.size(); iplot++)
      set_options (plots[iplot], options);

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.empty()) {
    cout << "psrplot: please specify filename[s]" << endl;
    return -1;
  } 

  if (cpgopen(plot_device.c_str()) < 0) {
    cout << "psrplot: Could not open plot device" << endl;
    return -1;
  }

  cpgask(1);

  if (n1 > 1 || n2 > 1)
    cpgsubp(n1,n2);

  Interpreter preprocessor;

  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    Reference::To<Archive> archive;
    archive = Archive::load( filenames[ifile] );

    archive->remove_baseline();

    if (jobs.size()) {
      if (verbose)
	cerr << "psrplot: preprocessing " << filenames[ifile] << endl;
      preprocessor.set(archive);
      preprocessor.script(jobs);
    }

    if (verbose)
      cerr << "psrplot: plotting " << filenames[ifile] << endl;

    if (overlay)
      cpgpage();

    for (unsigned iplot=0; iplot < plots.size(); iplot++) {
      if (!overlay)
	cpgpage ();
      plots[iplot]->plot (archive);
    }


  }
  catch (Error& error) {
    cerr << "Error while handling '" << filenames[ifile] << "'" << endl
	 << error.get_message() << endl;
  }

  cpgend();

  return 0;

}
catch (Error& error) {
  cerr << "psrplot: " << error << endl;
  return -1;
}

void help_plot_types ()
{
  cout << "Available Plots:\n" << factory.help() << endl;
}

void help_options (TextInterface::Class* tui)
{
  cout << tui->help() << endl;
}

void help_plot_options (const char* name)
{
  Plot* plot = factory.construct(name);
  help_options( plot->get_interface() );
}

void help_frame_options (const char* name)
{
  Plot* plot = factory.construct(name);
  help_options( plot->get_frame_interface() );
}

void set_options (Pulsar::Plot* plot, const vector<string>& options)
{
  TextInterface::Class* tui = plot->get_interface();
  TextInterface::Class* fui = plot->get_frame_interface();

  for (unsigned j = 0; j < options.size(); j++) {
    try {
      tui->process (options[j]);
    }
    catch (Error& error) {
      try {
	fui->process (options[j]);
      }
      catch (Error& error) {
	cerr << "psrplot: Invalid option '" << options[j] << "' " 
	     << error.get_message() << endl;
	exit (-1);
      }
    }
  }
}

// parses index from optarg and removes it from the string
unsigned get_index (string& optarg, vector<Plot*>& plots)
{
  unsigned index = fromstring<unsigned> ( stringtok (optarg, ":") );
  if (index >= plots.size()) {
    cerr << "psrplot: invalid plot index = " << index
	 << " nplot=" << plots.size() << endl;
    exit(-1);
  }
  return index;
}

// load the string of options into one of the plots
void specific_options (string optarg, vector<Plot*>& plots)
{
  // get the plot index
  unsigned index = get_index (optarg, plots);

  // separate the options
  vector<string> options;
  separate (optarg, options, ",");

  // set them for the specified plot
  set_options (plots[index], options);
}

// load the style file into one of the plots
void specific_style (string optarg, vector<Plot*>& plots)
{
  // get the plot index
  unsigned index = get_index (optarg, plots);

  // load the options
  vector<string> options;
  loadlines (optarg, options);

  // set them for the specified plot
  set_options (plots[index], options);
}
