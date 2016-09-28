/***************************************************************************
 *
 *   Copyright (C) 2008 - 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/Archive.h"

using namespace Pulsar;

//
//! An pcal of an application
//
class pcal : public Pulsar::Application
{
public:

  //! Default constructor
  pcal ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  void set_index (unsigned i) { index = i; }

protected:

  //! Index of model parameter to extract
  unsigned index;

  //! Add command line options
  void add_options (CommandLine::Menu&);
};



pcal::pcal ()
  : Application ("pcal", "pcal psrchive program")
{
  index = 0;
}


/*!

  Add application-specific command-line options.

*/

void pcal::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add an option that enables the user to set the scale with -s
  arg = menu.add (this, &pcal::set_index, 'i', "index");
  arg->set_help ("index of calibrator model parameter to extract");
}


/*!

  Scale every profile and optionally set the source name

*/

void pcal::process (Pulsar::Archive* archive)
{
  Reference::To<PolnCalibratorExtension> ext;
  ext = archive->get<PolnCalibratorExtension>();

  if (!ext)
    throw Error (InvalidParam, "pcal::process",
		 "archive does not contain a PolnCalibratorExtension");

  MJD epoch = ext->get_epoch();
  string mjd = epoch.printdays(4);

  unsigned nchan = ext->get_nchan();
  unsigned nparam = ext->get_nparam();
  
  if (index >= nparam)
    throw Error (InvalidState, "pcal::process",
		 "calibrator parameter index=%u >= nparam=%u", index, nparam);

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!ext->get_valid(ichan))
      continue;

    PolnCalibratorExtension::Transformation* xform;
    xform = ext->get_transformation(ichan);

    if (!xform->get_valid())
      continue;

    double freq = ext->get_centre_frequency(ichan);
    double val = xform->get_param(index);
    double err = sqrt(xform->get_variance(index));

    cout << mjd << " " << freq << " " << val << " " << err << " " << endl;
  }
  
  cout << endl;
}


/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  pcal program;
  return program.main (argc, argv);
}

