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
#include "SmoothingSpline.h"
#include "Physical.h"
#include "strutil.h"

#include <cassert>

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

  //! Names of the files provided as input
  vector<string> input_filenames;

  //! Rotation measures determined for each file, if smooth_in_time is enabled
  vector< std::pair<MJD,Estimate<double>> > rotation_measures;

  //! Update rotation parameter of PolnCalibratorExtension
  /*! @return the rotation measure */
  double update (PolnCalibratorExtension*);

  //! Update Faraday rotation measure of Archive
  void update (Archive*);

  //! Compute the least-squares estimate of the RM for the currently loaded data
  void compute_rm();

  //! Model fit outputs
  Estimate<double> fit_psi0;
  Estimate<double> fit_rm;
  double fit_chisq = 0;
  unsigned fit_nfree = 0;
  MJD reference_epoch;
  SmoothingSpline spline;

  void add_options (CommandLine::Menu&) override;

  //! Multiple of inter-quartile range used to identify outliers in residuals
  float iterative_outlier_threshold = 0.0;

  //! Smooth using a penalized spline with smoothing factor determined by m-fold cross-validation
  bool smooth_in_time = false;

  //! Discard any RM estimate with reduced chisq greater than this threshold
  float reduced_chisq_threshold = 3.0;
};

pcmrm::pcmrm ()
  : Application ("pcmrm", "estimates RM from a set of pcm outputs")
{
}

//! Add command line options
void pcmrm::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg = 0;
  
  menu.add ("\n" "Output options:");

  arg = menu.add (iterative_outlier_threshold, "iqr", "threshold");
  arg->set_help ("set the iterative outlier excision threshold (multiple of IQR)");

  arg = menu.add (smooth_in_time, "smooth");
  arg->set_help ("smooth temporal variations of RM using a penalized spline");

  arg = menu.add (smooth_in_time, "chisq");
  arg->set_help ("RM estimate is discarded if reduced chi squared is above this threshold");
}

void pcmrm::process (Pulsar::Archive* archive)
{
  Reference::To<Pulsar::PolnCalibratorExtension> ext =
    archive->get<Pulsar::PolnCalibratorExtension>();

  unsigned nchan = ext->get_nchan();
  MJD epoch = ext->get_epoch();

  if (reference_epoch == 0.0)
    reference_epoch = epoch;

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    Reference::To<MEAL::Complex2> xform = Calibration::new_transformation(ext, ichan);
    if (!xform)
      continue;
    
    auto feed = dynamic_cast<Calibration::HasOrientation*>(xform.get());
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
  
  if (smooth_in_time)
  {
    compute_rm();

    cerr << archive->get_filename() << " ndat= " << psi.size() << " MJD= " << reference_epoch 
        << " RM= " << fit_rm.val << " +/- " << sqrt(fit_rm.var) 
        << " chisq= " << fit_chisq << " nfree= " << fit_nfree << endl;

    // detete the data because each file is fit independently
    psi.clear();
    lambda_sq.clear();

    float reduced_chisq = fit_chisq/fit_nfree;
    if (reduced_chisq_threshold && reduced_chisq > reduced_chisq_threshold)
    {
      cerr << "pcmrm: discarding RM derived from " << archive->get_filename() << " because reduced chisq=" << reduced_chisq
          << " is greater than threshold=" << reduced_chisq_threshold << endl;
      return;
    }

    rotation_measures.push_back( {epoch, fit_rm} );
  }

  // this line always executes in default (spectrum) mode
  // in temporal mode, it is executed only if the RM fit is successful
  input_filenames.push_back( archive->get_filename() );
}

void pcmrm::compute_rm()
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
  fit.iterative_outlier_threshold = iterative_outlier_threshold;
  fit.weighted_least_squares (yval, lambda_sq, wt);
  fit_rm = fit.scale;
  fit_psi0 = fit.offset;
  fit_chisq = fit.chisq;
  fit_nfree = fit.nfree;
  
  if (iterative_outlier_threshold)
  {
    unsigned count = 0;
    for (auto val: fit.masked_weights)
      if (val == 0.0)
        count ++;

    cerr << "pcmrm: " << count << " values out of " << wt.size() << " flagged as outliers" << endl;
  }
}

void pcmrm::finalize ()
{
  if (smooth_in_time)
  {
    unsigned ndat = rotation_measures.size();
    assert (input_filenames.size() == ndat);

    vector<double> data_x (ndat);
    vector<Estimate<double>> data_y (ndat);

    MJD min_epoch = rotation_measures[0].first;
    MJD max_epoch = rotation_measures[0].first;

    for (unsigned idat=0; idat < ndat; idat++)
    {
      data_y[idat] = rotation_measures[idat].second;
      MJD epoch = rotation_measures[idat].first;

      if (epoch < min_epoch)
        min_epoch = epoch;

      if (epoch > max_epoch)
        max_epoch = epoch;
    }

    reference_epoch = (min_epoch + max_epoch) / 2.0;

    for (unsigned idat=0; idat < ndat; idat++)
    {
      MJD epoch = rotation_measures[idat].first;
      MJD diff = epoch - reference_epoch;
      data_x[idat] = diff.in_days();
    }

    CrossValidatedSmoothing cross_validate;
    cross_validate.set_spline(&spline);
    cross_validate.fit (data_x, data_y);
  }
  else
  {
    compute_rm();
    cout << "ndat= " << psi.size() << "  MJD= " << reference_epoch 
        << "  RM= " << fit_rm.val << " +/- " << sqrt(fit_rm.var) 
        << "  chisq= " << fit_chisq << " nfree= " << fit_nfree << endl;
  }

  for (unsigned ifile=0; ifile < input_filenames.size(); ifile++)
  {
    string filename = input_filenames[ifile];

    Reference::To<Archive> archive = Archive::load(filename);

    update (archive);
    
    string new_filename = replace_extension (filename, ".rmc");
    archive->unload (new_filename);
  }
}

void pcmrm::update (Archive* archive)
{
  PolnCalibratorExtension* ext;
  ext = archive->get<PolnCalibratorExtension>();
  if (!ext)
    throw Error (InvalidState, "pcmrm::update", 
      archive->get_filename() + " does not have PolnCalibratorExtension");

  double rotation_measure = update (ext);

  archive->set_rotation_measure (rotation_measure);
  archive->set_faraday_corrected (true);
}

double pcmrm::update (PolnCalibratorExtension* ext)
{
  unsigned nchan = ext->get_nchan();

  // by default, assume a single global RM fit 
  double rotation_measure = fit_rm.val;

  if (smooth_in_time)
  {
    MJD epoch = ext->get_epoch();
    MJD diff = epoch - reference_epoch;
    double xval = diff.in_days();
    rotation_measure = spline.evaluate (xval);
  }
  
  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    Reference::To<MEAL::Complex2> xform = Calibration::new_transformation(ext, ichan);
    if (!xform)
      continue;
    
    auto feed = dynamic_cast<Calibration::HasOrientation*>(xform.get());
    if (!feed)
      continue;

    double freq_MHz = ext->get_centre_frequency(ichan);
    double lambda = Pulsar::speed_of_light / (freq_MHz * 1e6);

    double Faraday_orientation = rotation_measure * lambda * lambda;
    feed->offset_orientation(-Faraday_orientation);

    Calibration::copy (ext->get_transformation(ichan), xform.get());
  }

  return rotation_measure;
}

int main (int argc, char** argv)
{
  pcmrm program;
  return program.main (argc, argv);
}

