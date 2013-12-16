/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/FaradayRotation.h"

#include "Pulsar/SimplePolnProfile.h"
#include "Pulsar/RotatingVectorModelOptions.h"

#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarParameter.h"

#include "pairutil.h"

#include <fstream>

using namespace Pulsar;
using namespace std;

//! Pulsar modeling application
class psrsim: public Pulsar::Application
{
public:

  //! Default constructor
  psrsim ();

  //! Process the given archive
  void process (Pulsar::Archive*);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! Polarization profile simulator
  Reference::To<SimplePolnProfile> sim;

  //! Faraday rotation measure
  double rotation_measure;

};

int main (int argc, char** argv)
{
  psrsim program;
  return program.main (argc, argv);
}

psrsim::psrsim () :
  Pulsar::Application ("psrsim", "pulse profile simulation program")
{
  has_manual = false;
  add( new Pulsar::StandardOptions );

  sim = new SimplePolnProfile;
  rotation_measure = 0;
}


void psrsim::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("");

  RotatingVectorModelOptions rvm_options;
  rvm_options.set_model (sim->get_RVM());
  rvm_options.set_fit (true);
  rvm_options.add_options (menu);
}


void psrsim::process (Pulsar::Archive* data)
{
  if (verbose)
    cerr << "psrsim: using " << data->get_filename() << endl;

  // perform Faraday rotation
  FaradayRotation xform;
  xform.set_reference_wavelength( data->get_centre_frequency() );
  xform.set_measure( rotation_measure );
  xform.execute( data );
}


