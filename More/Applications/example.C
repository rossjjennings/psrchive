/***************************************************************************
 *
 *   Copyright (C) <update>YEAR by AUTHOR</update>
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/UnloadOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

using namespace std;

//
//! A PSRCHIVE application that <update>DOES THINGS</update>
//
class example : public Pulsar::Application
{
public:

  //! Default constructor
  example ();

  //! Process the given archive
  void process (Pulsar::Archive*) override;

protected:

  // <update> The following attributes are specific to this example

  //! Scale attribute used to rescale all of the data in the archive
  double scale = 1.0;

  //! Name used to reset the name of the source
  std::string name;

  // </update>

  //! Add command line options
  void add_options (CommandLine::Menu&) override;
};


/*!

  The constructor must set the name of the application and a short
  description of its purpose.  These are shown when the user types
  "example -h"

  This constructor makes use of

  - StandardOptions (-j -J etc.): an option set that provides standard
  preprocessing with the pulsar command language interpreter.

  - UnloadOptions (-e -m etc.): an option set that provides standard
  options for unloading data.

  This constructor also sets the default values of the attributes that
  are unique to the program.

*/

example::example ()
  : Application ("example", "example psrchive program")
{
  add( new Pulsar::StandardOptions );
  add( new Pulsar::UnloadOptions );
}


/*!

  Add application-specific command-line options.

*/

void example::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  // <update> The following attributes are specific to this example

  // add an option that enables the user to set the scale with -s
  arg = menu.add (scale, 's', "scale");
  arg->set_help ("multiply all amplitudes by 'scale'");

  // add an option that enables the user to set the source name with -name
  arg = menu.add (name, "name", "string");
  arg->set_help ("set the source name to 'string'");

  // </update>
}


/*!
  // <update> This method should be re-implemented

  Scale every profile and optionally set the source name

*/

void example::process (Pulsar::Archive* archive)
{
  if (!name.empty())
    archive->set_source (name);

  unsigned nsub = archive->get_nsubint();
  unsigned nchan = archive->get_nchan();
  unsigned npol = archive->get_npol();
  
  for (unsigned isub=0; isub < nsub; isub++)    
    for (unsigned ipol=0; ipol < npol; ipol++)
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
        Pulsar::Profile* profile = archive->get_Profile (isub, ipol, ichan);
        profile->scale(scale);
      }
}


/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  example program;
  return program.main (argc, argv);
}

