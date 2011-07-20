/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"

using namespace std;

//
//! Converts between file formats
//
class psrconv : public Pulsar::Application
{
public:

  //! Default constructor
  psrconv ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);
};

#if 0
    "Available output formats:" << endl;

  Pulsar::Archive::agent_list ();
#endif

static string output_format = "PSRFITS";
static string unload_cal_ext = ".cf";
static string unload_psr_ext = ".rf";

psrconv::psrconv ()
  : Application ("psrconv", "converts between file formats")
{
  add( new Pulsar::StandardOptions );
}

void psrconv::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  arg = menu.add (output_format, 'o', "format");
  arg->set_help ("Name of the output file format");
}

//
//
//
void psrconv::process (Pulsar::Archive* archive)
{
  // ensure that the selected output format can be written
  Reference::To<Pulsar::Archive> output = Pulsar::Archive::new_Archive (output_format);  
  if (!output->can_unload())
    throw Error (InvalidParam, "psrconv", output_format + " unload method not implemented");

  if (verbose)
    cerr << "Converting " << archive->get_filename() << endl;

  output-> copy (*archive);

  if (verbose)
    cerr << "Conversion complete" << endl;
    
  if (verbose)
  {
      cerr << "Source: " << output -> get_source() << endl;
      cerr << "Frequency: " << output -> get_centre_frequency() << endl;
      cerr << "Bandwidth: " << output -> get_bandwidth() << endl;
      cerr << "# of subints: " << output -> get_nsubint() << endl;
      cerr << "# of polns: " << output -> get_npol() << endl;
      cerr << "# of channels: " << output -> get_nchan() << endl;
      cerr << "# of bins: " << output -> get_nbin() << endl;
  }
}

