/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"

#include "Pulsar/Archive.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "ChiSquared.h"
#include "Physical.h"
#include "strutil.h"

using namespace std;
using namespace Pulsar;

//
//! An example of an application
//
class pcmrm : public Pulsar::Application
{
public:

  //! Default constructor
  pcmrm ();

  //! Load rotation parameter from PolnCalibrator of the given pcm output
  void process (Pulsar::Archive*);

  //! Fit for RM and remove the trend
  void finalize();

protected:

  //! Rotation parameter index
  unsigned iparam_rot;

  //! Position angle
  std::vector< Estimate<double> > psi;
  //! wavelength squared
  std::vector< double > lambda_sq;

  vector<string> input_filenames;

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! Update rotation parameter of PolnCalibratorExtension
  void update (PolnCalibratorExtension*);

  double fit_psi0;
  double fit_rm;
};



pcmrm::pcmrm ()
  : Application ("pcmrm", "estimates RM from a set of pcm outputs")
{
  // default set up for bri00e19
  iparam_rot = 6;
}


void pcmrm::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (iparam_rot, 'i', "index");
  arg->set_help ("model index of rotation about line of sight");
}


void pcmrm::process (Pulsar::Archive* archive)
{
  Reference::To<Pulsar::PolnCalibratorExtension> ext =
    archive->get<Pulsar::PolnCalibratorExtension>();

  unsigned nchan = ext->get_nchan();

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    Estimate<double> rot;
    rot = ext->get_transformation(ichan)->get_Estimate(iparam_rot);

    if (rot.var == 0.0)
      continue;
    
    double freq_MHz = ext->get_centre_frequency(ichan);
    double lambda = Pulsar::speed_of_light / (freq_MHz * 1e6);

    psi.push_back (rot);
    lambda_sq.push_back (lambda*lambda);
  }
  
  input_filenames.push_back( archive->get_filename() );
}

void pcmrm::finalize ()
{
  unsigned ndat = psi.size();
  
  vector<double> yval (ndat);
  vector<double> wt (ndat);

  for (unsigned idat=0; idat < ndat; idat++)
  {
    wt[idat] = 1.0 / psi[idat].var;
    yval[idat] = psi[idat].val;
  }

  weighted_linear_fit (fit_rm, fit_psi0, yval, lambda_sq, wt);

  cerr << "pcmrm::finalize ndat=" << ndat << " RM=" << fit_rm << endl;
  
  for (unsigned ifile=0; ifile < input_filenames.size(); ifile++)
  {
    string filename = input_filenames[ifile];

    Reference::To<Archive> archive = Archive::load(filename);

    PolnCalibratorExtension* ext;
    ext = archive->get<PolnCalibratorExtension>();
    if (!ext)
      throw Error (InvalidState, "pcmrm::finalize", 
		   filename + " does not have PolnCalibratorExtension");

    update (ext);

    archive->set_rotation_measure (fit_rm);
    archive->set_faraday_corrected (true);
    
    string new_filename = replace_extension (filename, ".rmc");
    archive->unload (new_filename);
  }
}

void pcmrm::update (PolnCalibratorExtension* ext)
{
  unsigned nchan = ext->get_nchan();

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    Estimate<double> rot;
    rot = ext->get_transformation(ichan)->get_Estimate(iparam_rot);

    if (rot.var == 0.0)
      continue;
    
    double freq_MHz = ext->get_centre_frequency(ichan);
    double lambda = Pulsar::speed_of_light / (freq_MHz * 1e6);

    rot.val -= fit_psi0 + fit_rm * lambda * lambda;

    ext->get_transformation(ichan)->set_Estimate(iparam_rot, rot);
  }
}

int main (int argc, char** argv)
{
  pcmrm program;
  return program.main (argc, argv);
}

