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
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/FaradayRotation.h"

#include "Pulsar/SimplePolnProfile.h"
#include "Pulsar/RotatingVectorModelOptions.h"
#include "Pulsar/Feed.h"

#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ScalarParameter.h"

#include "pairutil.h"

#include <fstream>
#include <iomanip>

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

  //! Number of Pulse Profiles to be output 
  int profile_number;

  //! Simulate observation of the input data with a specified receiver
  bool simulate_reception;
  
  //! Orientation and ellipticity of single receptor
  std::pair<double,double> receptor_angles;

  void set_receptor_angles (std::pair<double,double> val)
  {
    receptor_angles = val;
    simulate_reception = true;
  }
};

int main (int argc, char** argv)
{
  psrsim program;
  return program.main (argc, argv);
}

psrsim::psrsim () :
  Pulsar::Application ("psrsim", "pulse profile simulation program"),
  receptor_angles (0.0, 0.0)
{
  has_manual = false;
  add( new Pulsar::StandardOptions );

  sim = new SimplePolnProfile;
  rotation_measure = 0;

  profile_number = 1;

  simulate_reception = false;
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

  arg = menu.add (sim.get(), &SyntheticPolnProfile::set_swims, 's');
  arg->set_help ("peak standard deviation of simulated SWIMS");

  arg = menu.add (sim.get(), &SyntheticPolnProfile::set_baseline, 'b');
  arg->set_help ("off-pulse baseline mean");

  RotatingVectorModelOptions rvm_options;
  rvm_options.set_model (sim->get_RVM());
  rvm_options.add_options (menu);

  menu.add ("\n" "Simulation Options");

  arg = menu.add (profile_number, 'N');
  arg->set_help ("number of pulse profiles to be simulated");

  arg = menu.add (this, &psrsim::set_receptor_angles, 'R', "or:el");
  arg->set_help ("orientation and ellipticity of receptor 0 [deg]");
}


void psrsim::process (Pulsar::Archive* data)
{
  if (verbose)
    cerr << "psrsim: using " << data->get_filename() << endl;

  unsigned nsubint = data->get_nsubint();
  unsigned nchan = data->get_nchan();
  unsigned npol = data->get_npol();
  
  if (simulate_reception)
  {
    if (npol != 4)
      throw Error (InvalidState, "psrsim:process",
		   "simulating reception requires full-polarization input");

    Calibration::Feed receiver;
    receiver.set_orientation (0, receptor_angles.first * M_PI/180);
    receiver.set_orientation (1, receptor_angles.first * M_PI/180);

    receiver.set_ellipticity (0, receptor_angles.second * M_PI/180);
    receiver.set_ellipticity (1, receptor_angles.second * M_PI/180);

    Jones<double> J = receiver.evaluate();

    cerr << "psrsim: simulated response = " << J << endl;

    data->convert_state (Signal::Coherence);    

    for (unsigned isub=0; isub < nsubint; isub++)
      data->get_Integration(isub)->expert()->transform (J);

    string unload = "psrsim_reception.ar";
    data->unload (unload);

    cerr << "psrsim: simulated observation written to " << unload << endl;
    return;
  }

  for (int i=0; i<profile_number; i++)
  {
    ostringstream value;
    value << setfill('0') << setw(12) << i;
    string unload = value.str() + string("_psrsim.ar");
  
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
    
    data->unload (unload);
    
  } // end of for loop to generate multiple files
  
}


