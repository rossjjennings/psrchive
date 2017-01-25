/***************************************************************************
 *
 *   Copyright (C) 2009 - 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/PlotOptions.h"

#include "Pulsar/PlotFactory.h"
#include "Pulsar/FramedPlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/ProfileShiftFit.h"

#include <cpgplot.h>

using namespace std;
using namespace Pulsar;

//
//! An application to interactively trash files
//
class trash : public Pulsar::Application
{
public:

  //! Default constructor
  trash ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Process any remaining archives
  void finalize ();

  //! Set the standard profile used to automatically trash
  void set_standard (const std::string& filename);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  // -c add plot options
  void add_plot_options (const string& name);

  //! Automatically discard profiles that do not fit the template well
  Reference::To<ProfileShiftFit> fit;
  //! Threshold used for automatic template-based trash algorithm
  double fit_threshold;
  
  unsigned plotted;
  string plot_name;

  // Plot options set with -c
  vector<string> options;

  // Available plots
  PlotFactory factory;

  // Plot classes to be used
  Reference::To<Plot> plot;

  // Plot options
  Pulsar::PlotOptions plot_options;
  Pulsar::StandardOptions standard_options;

  // List of archives plotted
  vector< pair<bool,string> > files;

  void select ();
};


trash::trash ()
  : Application ("trash", "quickly visually inspect and trash files")
{
  plot_options.set_x_npanel (5);
  plot_options.set_y_npanel (4);
  plot_options.set_device ("/xs");

  // by default, pscrunch, tscrunch, and fscrunch
  //standard_options.add_default_job ("pTFC");

  // by default, plot the total intensity
  plot_name = "flux";

  add( &plot_options );
  add( &standard_options );

  plotted = 0;

  fit_threshold = 2.0;
}

void trash::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (plot_name, 'p', "plot");
  arg->set_help ("set plot type");

  arg = menu.add (this, &trash::add_plot_options, 'c', "cfg[s]");
  arg->set_help ("add plot options");

  arg = menu.add (this, &trash::set_standard, 'S', "std.ar");
  arg->set_help ("use goodness of template fit");
}

void trash::add_plot_options (const std::string& arg)
{
  separate (arg, options, ",");
}

//! Set the standard profile used to automatically trash
void trash::set_standard (const std::string& filename)
{
  Reference::To<Archive> std = Pulsar::Archive::load (filename);
  std = std->total();

  fit = new ProfileShiftFit;
  fit->choose_maximum_harmonic = true;
  fit->set_standard (std->get_Profile(0,0,0));
}

void trash::process (Pulsar::Archive* archive)
{
  if (fit)
  {
    fit->set_Profile( archive->get_Profile(0,0,0) );
    if (fit->get_reduced_chisq() > fit_threshold)
      cout << "trash: " << archive->get_filename()
	   << " fit=" << fit->get_reduced_chisq() << endl;

    return;
  }

  cpgpage ();
  cpgask (0);

  if (!plot)
  {
    plot = factory.construct( plot_name );
    
    FramedPlot* framed = dynamic_cast<FramedPlot*> (plot.get());

    if (framed)
    {
      // make the plot larger
      framed -> get_frame() -> set_viewport (0.05, 0.95, 0.05, 0.95);
      
      // remove the above frame labels
      framed -> get_frame() -> get_label_above()->set_all ("");
    }

    for (unsigned iopt=0; iopt < options.size(); iopt++)
      plot->configure( options[iopt] );
  }

  plot->preprocess (archive);
  plot->plot (archive);

  if (files.size() == 0)
    files.resize( plot_options.get_x_npanel() * plot_options.get_y_npanel() );

  files[plotted].first = false;
  files[plotted].second = archive->get_filename();

  plotted ++;

  if (plotted == files.size())
  {
    select ();
    plotted = 0;
  }
}

void trash::finalize ()
{
  if (plotted)
  {
    select ();
    plotted = 0;
  }
}

void trash::select ()
{
  char ans='j';
  float x=0.5, y=0.5;

  unsigned xpanel = plot_options.get_x_npanel();
  unsigned ypanel = plot_options.get_y_npanel();

  while (ans != ' ')
  {
    cpgpanl (1,1);
    cpgsvp (0,1, 0,1);
    // left-to-right top-to-bottom
    cpgswin (0,1, 1,0);
    cpgcurs (&x,&y,&ans);

    if (ans == 'A')
    {
      if (x < 0) x = 0;
      if (y < 0) y = 0;

      unsigned xip = unsigned (x);
      unsigned yip = unsigned (y);

      if (xip >= xpanel) xip = xpanel - 1;
      if (yip >= ypanel) yip = ypanel - 1;

      unsigned index = yip * xpanel + xip;

      if (index >= plotted)
	continue;

      files[index].first = !files[index].first;
      
      cpgpanl (xip+1, yip+1);

      if (files[index].first)
	cpgsci (2);
      else
	cpgsci (0); // lines may be "erased" by overwriting them
                    // with color index 0 (if the device permits this).
      
      cpgmove (0,0);
      cpgdraw (1,1);
      cpgmove (0,1);
      cpgdraw (1,0);

    }
    else if (ans == 'a' || ans == 'e')
    {
      cerr << "psrtrash: trashing all plotted" << endl;
      for (unsigned i=0; i<plotted; i++)
        cout << "trash: " << files[i].second << endl;

      break;
    }
  }

  for (unsigned ifile=0; ifile<files.size(); ifile++)
    if (files[ifile].first)
      cout << "trash: " << files[ifile].second << endl;

  cpgpanl (xpanel,ypanel);
}

int main (int argc, char** argv)
{
  trash program;
  return program.main (argc, argv);
}

