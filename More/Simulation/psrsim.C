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

// defined in RotatingVectorModelOptions.C
double deg_to_rad (const std::string& arg);

void psrsim::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add ("\n" "Interstellar medium options:");

  arg = menu.add (rotation_measure, 'r');
  arg->set_help ("rotation measure");

  menu.add ("\n" "Pulse shape and statistics:");

  MEAL::ScaledVonMises* svm = sim->get_Intensity();

  arg = menu.add (svm, &MEAL::ScaledVonMises::set_width,
		  deg_to_rad, 'w', "degrees");
  arg->set_help ("width of pulse in degrees");

  arg = menu.add (sim.get(), &SyntheticPolnProfile::set_noise, 'n');
  arg->set_help ("standard deviation of noise");
		  
  RotatingVectorModelOptions rvm_options;
  rvm_options.set_model (sim->get_RVM());
  rvm_options.add_options (menu);
}


void psrsim::process (Pulsar::Archive* data)
{
  if (verbose)
    cerr << "psrsim: using " << data->get_filename() << endl;

  unsigned nsubint = data->get_nsubint();
  unsigned nchan = data->get_nchan();
  unsigned npol = data->get_npol();

  if (npol != 4)
  {
    npol = 4;
    data->resize( nsubint, npol, nchan );
  }

  data->set_state (Signal::Stokes);
  data->set_poln_calibrated (true);

  for (unsigned isub=0; isub < nsubint; isub++)
  {
    Integration* subint = data->get_Integration(isub);

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      sim->get_PolnProfile(subint->new_PolnProfile(ichan));
      subint->set_weight(ichan, 1.0);
    }
  }

  if (rotation_measure != 0.0)
  {
    // perform Faraday rotation
    FaradayRotation xform;
    xform.set_reference_frequency( data->get_centre_frequency() );
    xform.set_measure( -rotation_measure );
    xform.just_do_it( data );
  }

  data->set_faraday_corrected (false);
  data->set_rotation_measure (rotation_measure);
  data->set_dispersion_measure (0.0);

  data->unload ("psrsim.ar");
}


