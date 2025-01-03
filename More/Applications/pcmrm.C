/***************************************************************************
 *
 *   Copyright (C) 2021 - 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"

#include "Pulsar/Archive.h"
#include "Pulsar/PolnCalibratorExtensionUtils.h"
#include "Pulsar/HasOrientation.h"
#include "MEAL/Complex2.h"

#include "LinearRegression.h"
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

  //! Position angle
  std::vector< Estimate<double> > psi;
  //! wavelength squared
  std::vector< double > lambda_sq;

  vector<string> input_filenames;

  //! Update rotation parameter of PolnCalibratorExtension
  void update (PolnCalibratorExtension*);

  Estimate<double> fit_psi0;
  Estimate<double> fit_rm;

  void add_options (CommandLine::Menu&) override { /* none */ }
};

pcmrm::pcmrm ()
  : Application ("pcmrm", "estimates RM from a set of pcm outputs")
{
}

void pcmrm::process (Pulsar::Archive* archive)
{
  Reference::To<Pulsar::PolnCalibratorExtension> ext =
    archive->get<Pulsar::PolnCalibratorExtension>();

  unsigned nchan = ext->get_nchan();

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    MEAL::Complex2* xform = Calibration::new_transformation(ext, ichan);
    if (!xform)
      continue;
    
    auto feed = dynamic_cast<Calibration::HasOrientation*>(xform);
    if (!feed)
      continue;

    Estimate<double> rot;
    rot = feed->get_orientation();

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

  LinearRegression fit;
  fit.weighted_least_squares (yval, lambda_sq, wt);
  fit_rm = fit.scale;
  fit_psi0 = fit.offset;

  MJD epoch;

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

    archive->set_rotation_measure (fit_rm.val);
    archive->set_faraday_corrected (true);
    
    string new_filename = replace_extension (filename, ".rmc");
    archive->unload (new_filename);

    if (epoch == 0.0)
      epoch = ext->get_epoch();
  }

  cout << "ndat= " << ndat << "  MJD= " << epoch 
       << "  RM= " << fit_rm.val << " +/- " << sqrt(fit_rm.var) << endl;
}

void pcmrm::update (PolnCalibratorExtension* ext)
{
  unsigned nchan = ext->get_nchan();

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    MEAL::Complex2* xform = Calibration::new_transformation(ext, ichan);
    if (!xform)
      continue;
    
    auto feed = dynamic_cast<Calibration::HasOrientation*>(xform);
    if (!feed)
      continue;

    double freq_MHz = ext->get_centre_frequency(ichan);
    double lambda = Pulsar::speed_of_light / (freq_MHz * 1e6);

    double Faraday_orientation = fit_psi0.val + fit_rm.val * lambda * lambda;
    feed->offset_orientation(-Faraday_orientation);

    Calibration::copy (ext->get_transformation(ichan), xform);
  }
}

int main (int argc, char** argv)
{
  pcmrm program;
  return program.main (argc, argv);
}

