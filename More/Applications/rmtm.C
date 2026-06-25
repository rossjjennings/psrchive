/***************************************************************************
 *
 *   Copyright (C) 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/SpectralMETM.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

using namespace std;

//
//! A PSRCHIVE application that <update>DOES THINGS</update>
//
class rmtm : public Pulsar::Application
{
public:

  //! Default constructor
  rmtm ();

  //! Process the given archive
  void process (Pulsar::Archive*) override;

  //! Set the filename of the standard / template
  void set_standard (const std::string&);

protected:

  Pulsar::SpectralMETM fmtm;

  //! Add command line options
  void add_options (CommandLine::Menu&) override;
};


rmtm::rmtm ()
  : Application ("rmtm", "rmtm psrchive program")
{
  add( new Pulsar::StandardOptions );
  add( new Pulsar::UnloadOptions );
}

void rmtm::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (this, &rmtm::set_standard, 's', "std.ar");
  arg->set_help ("filename of the standard/template");
}

void rmtm::set_standard (const std::string& filename)
{
  Reference::To<Pulsar::Archive> std = Pulsar::Archive::load(filename);
  fmtm.set_standard(std);
}

void rmtm::process (Pulsar::Archive* archive)
{
  fmtm.add_observation (archive);
  fmtm.solve();
}


/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  rmtm program;
  return program.main (argc, argv);
}

