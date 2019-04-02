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

#include "Pulsar/ComponentModel.h"
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

double pulse_width (const Profile* profile, double at_fraction_of_height);

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
  Reference::To<SimplePolnProfile> simulator;

  //! Faraday rotation measure
  double rotation_measure;

  //! Signal-to-noise ratio of integrated total
  double signal_to_noise_ratio;

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

  //! Set the total intensity profile to the ComponentModel
  void load_component_model (const std::string& filename);

  //! Set the signal-to-noise ratio of the integrated total
  void set_signal_to_noise_ratio (double snr) { signal_to_noise_ratio = snr; }
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

  simulator = new SimplePolnProfile;
  rotation_measure = 0;
  signal_to_noise_ratio = 0;

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

  MEAL::ScaledVonMises* svm = simulator->get_Intensity();

  arg = menu.add (svm, &MEAL::ScaledVonMises::set_width,
		  deg_to_rad, 'w', "degrees");
  arg->set_help ("width of pulse in degrees");

  arg = menu.add (this, &psrsim::load_component_model, 'P', "paas.m");
  arg->set_help ("load component model as output by paas");

  typedef void(SyntheticPolnProfile::*the_float_one)(float);

  arg = menu.add (simulator.get(), (the_float_one) &SyntheticPolnProfile::set_degree, 'L');
  arg->set_help ("degree of linear polarization");
  
  arg = menu.add (simulator.get(), &SyntheticPolnProfile::set_noise, 'n');
  arg->set_help ("standard deviation of noise");

  arg = menu.add (this, &psrsim::set_signal_to_noise_ratio, "snr");
  arg->set_help ("signal-to-noise ratio of integrated total");

  arg = menu.add (simulator.get(), &SyntheticPolnProfile::set_swims, 's');
  arg->set_help ("peak standard deviation of simulated SWIMS");

  arg = menu.add (simulator.get(), &SyntheticPolnProfile::set_baseline, 'B');
  arg->set_help ("off-pulse baseline mean");

  RotatingVectorModelOptions rvm_options;
  rvm_options.set_model (simulator->get_RVM());
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
  unsigned nbin = data->get_nbin();
 
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

  // if the S/N is specified, compute and set the noise in each frequency channel and sub-integration here
  if (signal_to_noise_ratio != 0)
  {
    MEAL::ScaledVonMises* svm = simulator->get_Intensity();
    Profile intensity (nbin);
    simulator->set_Profile (&intensity, svm);

    double area = intensity.sum();
    double width = pulse_width (&intensity, 0.05);
    double sigma = area / (signal_to_noise_ratio * sqrt(width));
    sigma *= sqrt(nchan * nsubint);
    simulator->set_noise( sigma );
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
	simulator->get_PolnProfile(subint->new_PolnProfile(ichan));
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


void psrsim::load_component_model (const std::string& filename) try
{
  Reference::To<Pulsar::ComponentModel> component_model
    = new Pulsar::ComponentModel(filename);

  simulator->set_total_intensity (component_model->get_model());
}
catch (Error& error)
{
  cerr << error << endl;
  throw error += "psrsim::load_component_model";
}

double pulse_width (const Profile* profile, double at_fraction_of_height)
{
  int nbin = profile->get_nbin();
  const float* amps = profile->get_amps();

  double max = profile->max();
  int max_bin = profile->find_max_bin();

  int start_bin = max_bin;
  while (max_bin - start_bin < nbin && amps[(start_bin+nbin)%nbin] > max*at_fraction_of_height) start_bin --;

  int end_bin = max_bin;
  while (end_bin - max_bin < nbin && amps[(end_bin+nbin)%nbin] > max*at_fraction_of_height) end_bin ++;

  return end_bin - start_bin;
}

