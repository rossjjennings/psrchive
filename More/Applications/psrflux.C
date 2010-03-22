/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/DynamicSpectrum.h"
#include "Pulsar/StandardFlux.h"

using namespace Pulsar;

//
//! Produce dynamic spectra from Archives
//
class psrflux : public Pulsar::Application
{
public:

  //! Default constructor
  psrflux ();

  //! One-time setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Extension to append
  std::string ext;

  //! Flux calculation
  DynamicSpectrum ds;

  //! Standard profile file
  std::string stdfile;

  //! Standard archive
  Reference::To<Archive> stdarch;

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

};


psrflux::psrflux ()
  : Application ("psrflux", "Produce dynamic spectra from Archives")
{
  ext = "ds";
  stdfile = "";
}

void psrflux::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (ext, 'e', "ext");
  arg->set_help ("Append extention to output (default .ds)");

  arg = menu.add (stdfile, 's', "std");
  arg->set_help ("Standard profile file");

}

void psrflux::setup ()
{

  // We need a standard for now.  Eventually different methods
  // will be allowed.
  if (stdfile=="") 
    throw Error(InvalidState, "psrflux", 
        "No standard specified");

  // Load std
  stdarch = Archive::load(stdfile);
  stdarch->convert_state(Signal::Intensity);
  stdarch->total();

  // Set up DS calculation
  Reference::To<StandardFlux> flux = new StandardFlux;
  flux->set_standard(stdarch->get_Profile(0,0,0));
  ds.set_flux_method(flux);
  
}

void psrflux::process (Pulsar::Archive* archive)
{

  // Convert to total intensity
  archive->convert_state(Signal::Intensity);

  // Compute DS
  ds.set_Archive(archive);
  ds.compute();

  // Unload archive with .sm extension
  std::string outf = archive->get_filename() + "." + ext;
  cout << "Unloading " << outf << endl;
  ds.unload(outf);

}

static psrflux program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

