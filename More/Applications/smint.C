/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
   smint - Smooth and interpolate data, where data might be
         - MEM/METM model parameters, including PolnCalibratorExtension
           and CalibratorStokes parameters
         - Pulsar spectrum (e.g. in each phase bin)
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/FluxCalibratorExtension.h"

#include "Pulsar/CalibrationInterpolatorExtension.h"
#include "Pulsar/CalibratorTypes.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Profile.h"

// #undef HAVE_PGPLOT

#ifdef HAVE_PGPLOT
#include <cpgplot.h>
#include "EstimatePlotter.h"
#endif

#ifdef HAVE_SPLINTER
#include "Pulsar/CalibrationInterpolator.h"
#include "Pulsar/SplineSmooth.h"
#endif

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

#include "UnaryStatistic.h"
#include "strutil.h"
#include "SmoothingSpline.h"

#include <fstream>

using namespace Pulsar;
using namespace std;

//
//! Smooth and interpolate data
//
class smint : public Pulsar::Application
{
public:

  //! Default constructor
  smint ();

  //! Load data from the archive
  void process (Pulsar::Archive*);

  //! Perform the fit
  void finalize ();

  //! Set up archive to store interpolator
  void prepare_solution (Archive*);

protected:

  Reference::To< CalibrationInterpolatorExtension > result;
  
  bool convert_epochs;
  bool use_native_scale;

  void fit_polynomial (const vector< double >& data_x,
                       const vector< Estimate<double> >& data_y);

#if HAVE_PGPLOT
  void plot_data (const vector< double >& data_x,
                  const vector< Estimate<double> >& data_y,
                  const string& filename);

  void plot_model (std::pair<MEAL::Axis<double>*, MEAL::Scalar*>& model,
		   unsigned npts, double xmin, double xmax);
#endif

  //! Add command line options
  void add_options (CommandLine::Menu&);

  // determine smoothing by minimizing generalized cross-validation
  bool minimize_gcv;
  // determine smoothing by minimizing true mean-squared error
  bool minimize_tmse;

  // determine smoothing by m-fold cross-validation
  bool cross_validate;

  CrossValidatedSmoothing* cross_validated_smoothing;

#if HAVE_SPLINTER
  CrossValidatedSmooth2D* cross_validated_smoothing_2D;
#endif

  bool unload_solution;
  bool unload_smoothed;
  
  // find the median effective number of free parameters
  bool find_median_nfree;

  double median_nfree;
  double pspline_alpha;

  double interquartile_range;
  double outlier_smoothing;

  bool interpolate;
  bool bootstrap_uncertainty;

  bool use_smoothing_spline ();
  
  unsigned freq_order;
  unsigned time_order;
  float threshold;

  string plot_device;

  // a spectrum of estimates observed at a given epoch
  class row
  {
  public:
    SmoothingSpline spline1d;

    string filename;

    MJD epoch;
    double x0; // coordinate shared by all data in row
    vector< double > freq;
    vector< Estimate<double> > data;
  };

  // a set of spectra from an indexed slice of data
  class set
  {
  public:

#if HAVE_SPLINTER
    SplineSmooth2D spline2d;
#endif

    unsigned index;
    std::vector<row> table;
  };

  // pulsar profile [ipol] to be smoothed
  vector<set> profile_data;

  // PolnCalibratorExtension [iparam] to be smoothed
  vector<set> pcal_data;

  // CalibratorStokes [ipol] to be smoothed
  vector<set> cal_stokes_data;

  // FluxCalibratorExtension [iparam] to be smoothed
  vector<set> fcal_data;

  // the centre frequency of the data
  double centre_frequency;
  vector<double> channel_frequency;

  string reference_filename;
  vector<string> input_filenames;

  // the reference epoch of the data
  MJD reference_epoch;
  MJD min_epoch;
  MJD max_epoch;
      
  // load profile data from Integration
  void load_data (vector<set>& data, Integration* subint);

  // load data from a container into one or more set::table
  template<class Container>
  void load_data (vector<set>& data, Container* ext, const MJD& epoch);

  // add parameters, from istart to iend inclusive, only if they have been measured
  template<class Container>
  void add_if_has_data (vector<set>& data, Container* ext,
                        unsigned istart, unsigned iend);

  template<typename Container>
  void set_reference (Pulsar::Archive* archive, Container* ext);

  template<typename Container>
  void check_reference (Pulsar::Archive* archive, Container* ext);

  // unload smoothed model values into container
  template<class Container>
  void unload (Container* ext, vector<set>& data, unsigned ifile);

#if HAVE_SPLINTER

  void unload (Integration* subint, vector<set>& data);
  void unload (Integration* subint, set& data, unsigned ipol, unsigned ibin);

#endif

  // unload one row of smoothed model values into container
  void unload_row (set& dataset, unsigned irow);

  // unload text to file
  void unload (const string& filename, const vector<row>& table);

  CalibrationInterpolatorExtension::Parameter::Type model_code;
  string spline_filename;
  string plot_filename;
  string data_filename;

  unsigned model_index;
  
  unsigned plot_npts;
  double plot_xmin;
  double plot_xmax;
  
  // fit 2-D data using a bunch of 1-D fits
  bool row_by_row;

  // the current sub-integration when fitting profile data
  Pulsar::Integration* current_subint;

  // the current polarization when fitting profile data
  unsigned current_ipol;
  
  void fit (set&);

  void fit_pspline (SmoothingSpline& spline,
		    vector< double >& data_x,
		    vector< Estimate<double> >& data_y);

  void iteratively_remove_outliers (SmoothingSpline& spline,
				    vector< double >& data_x,
				    vector< Estimate<double> >& data_y);
  
  unsigned remove_outliers (SmoothingSpline& spline,
			    vector< double >& data_x,
			    vector< Estimate<double> >& data_y);

#if HAVE_PGPLOT

  template<class Spline>
    void setup_and_plot (Spline& spline,
			 const vector< double >& data_x,
			 const vector< Estimate<double> >& data_y);

  void plot_model (SmoothingSpline& spline,
                   unsigned npts, double xmin, double xmax);
#endif

#if HAVE_SPLINTER

  void fit_pspline (SplineSmooth2D& spline, vector<row>& table);
  Reference::To<CalibrationInterpolator> previous_interpolator;

#if HAVE_PGPLOT
  void plot_model (SplineSmooth2D& spline, double x0,
                   unsigned npts, double xmin, double xmax);
#endif

  void load_previous_solution (const string& filename);

  Reference::To<Archive> previous_solution;

  void compare (const Archive*);

  template<class Container>
  void compare (const Archive*,
                CalibrationInterpolatorExtension::Parameter::Type);

  template<class Container>
  double compute_gof (const Container* ext, const Container* prev, 
                       unsigned iparam);

  template <class Extension>
  void update (const Extension* ext)
  { previous_interpolator->update (ext); }

  // no update required - CalibratorStokes depends on PolnCalibratorExtension
  void update (const CalibratorStokes* ext) { }

#endif

};


/*!


*/

smint::smint ()
  : Application ("smint", "smooth and interpolate")
{
  add( new Pulsar::StandardOptions );

  use_native_scale = false;
  freq_order = time_order = 0;
  threshold = 0.001;
  pspline_alpha = 0.0;
  row_by_row = false;
  minimize_gcv = false;
  minimize_tmse = false;
  find_median_nfree = false;
  median_nfree = 0;
  current_subint = 0;

  interquartile_range = 0.0;
  outlier_smoothing = 0.0;

  interpolate = true;

  cross_validate = false;
  cross_validated_smoothing = new CrossValidatedSmoothing;

#if HAVE_SPLINTER
  cross_validated_smoothing_2D = new CrossValidatedSmooth2D;
#endif
  
  bootstrap_uncertainty = false;

  unload_solution = true;
  unload_smoothed = false;
}

/*!


*/

void smint::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  arg = menu.add (interpolate, "noint");
  arg->set_help ("disable interpolation (smooth only)");

  arg = menu.add (bootstrap_uncertainty, "boot");
  arg->set_help ("output bootstrap (replacement) error bars");

  // add a blank line and a header to the output of -h
  menu.add ("\n" "Polynomial fitting options:");

  arg = menu.add (freq_order, 'f', "int");
  arg->set_help ("order of polynomial along radio frequency");

  arg = menu.add (time_order, "t", "int");
  arg->set_help ("order of polynomial along time");

  arg = menu.add (row_by_row, "1");
  arg->set_help ("fit 2-D data using series of 1-D fits");
  
  // add a blank line and a header to the output of -h
  menu.add ("\n" "Penalized spline (p-spline) fitting options:");

  arg = menu.add (pspline_alpha, 'p', "double");
  arg->set_help ("p-spline smoothing factor 'alpha'");

  arg = menu.add (minimize_gcv, "gcv");
  arg->set_help ("compute p-spline smoothing by minimizing GCV");

  arg = menu.add (minimize_tmse, "mse");
  arg->set_help ("compute p-spline smoothing by minimizing true MSE");

  arg = menu.add (find_median_nfree, "mnf");
  arg->set_help ("compute p-spline smoothing using median nfree");

  arg = menu.add (cross_validate, "cross");
  arg->set_help ("compute p-spline smoothing using m-fold cross-validation");

#if HAVE_SPLINTER

  arg = menu.add (cross_validated_smoothing_2D, &CrossValidatedSmooth2D::set_npartition, "cross-m");
  arg->set_help ("number of cross-validation partitions/iterations, m"
                 " (default: " + tostring(cross_validated_smoothing_2D->get_npartition()) + ")");

  arg = menu.add (cross_validated_smoothing_2D, &CrossValidatedSmooth2D::set_validation_fraction, "cross-f");
  arg->set_help ("fraction of data used to validate on each iteration"
                 " (default: " + tostring(cross_validated_smoothing_2D->get_validation_fraction ()) + ")");

  arg = menu.add (cross_validated_smoothing_2D, &CrossValidatedSmooth2D::set_iqr_threshold, "cross-iqr");
  arg->set_help ("inter-quartile range threshold"
                 " (default: " + tostring(cross_validated_smoothing_2D->get_iqr_threshold ()) + ")");

  arg = menu.add (cross_validated_smoothing_2D, &CrossValidatedSmooth2D::set_gof_step_threshold, "cross-gof");
  arg->set_help ("step in goodness-of-fit threshold"
                 " (default: " + tostring(cross_validated_smoothing_2D->get_gof_step_threshold ()) + ")");

  // add a blank line and a header to the output of -h
  menu.add ("\n" "Comparison and verification options:");

  arg = menu.add (this, &smint::load_previous_solution, "gof", "solution");
  arg->set_help ("compare data to spline solution");

#endif

  // add a blank line and a header to the output of -h
  menu.add ("\n" "Iterative outlier excision options:");

  arg = menu.add (interquartile_range, "iqr", "double");
  arg->set_help ("outlier threshold as inter-quartile range");

#if HAVE_PGPLOT
  // add a blank line and a header to the output of -h
  menu.add ("\n" "Plotting options:");

  arg = menu.add (plot_device, 'D', "dev");
  arg->set_help ("print all plots to a single device");
#endif

}

bool smint::use_smoothing_spline ()
{
  return minimize_gcv || minimize_tmse || find_median_nfree
    || pspline_alpha > 0 || cross_validate;
}

/*!


*/

template<class Container>
void smint::load_data (vector<set>& data, Container* ext, const MJD& epoch)
{
  for (unsigned i=0; i<data.size(); i++)
  {
    data[i].table.resize( data[i].table.size() + 1 );

    std::vector< double >& data_x = data[i].table.back().freq;
    std::vector< Estimate<double> >& data_y = data[i].table.back().data;

    data[i].table.back().filename = data_filename;

    data[i].table.back().epoch = epoch;

    unsigned nchan = ext->get_nchan();
    unsigned iparam = data[i].index;

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      Estimate<float> val (0.0, 0.0);
      
      if (ext->get_valid(ichan))
	val = ext->get_Estimate (iparam, ichan);

      data_x.push_back( channel_frequency[ichan] );
      data_y.push_back( val );
    }
  }
}

template<class Container>
void smint::add_if_has_data (vector<set>& data, Container* ext, 
                             unsigned istart, unsigned iend)
{
  const unsigned nchan = ext->get_nchan();

  for (unsigned iparam=istart; iparam <= iend; iparam++)
  {
    bool has_data = false; 

    for (unsigned ichan=0; ichan < nchan; ichan++) 
      if (ext->get_Estimate (iparam, ichan).var != 0.0)
        has_data = true;

    if (!has_data)
      continue;

    data.resize ( data.size() + 1 );
    data.back().index = iparam;
  }
}

template<typename Container>
void smint::set_reference (Pulsar::Archive* archive, Container* ext)
{
  centre_frequency = archive->get_centre_frequency();
  reference_filename = archive->get_filename();

  channel_frequency.resize( ext->get_nchan() );
  for (unsigned i=0; i<ext->get_nchan(); i++)
    channel_frequency[i] = ext->get_centre_frequency(i) - centre_frequency;
}

template<typename Container>
void smint::check_reference (Pulsar::Archive* archive, Container* ext)
{
  if (archive->get_centre_frequency() != centre_frequency)
    throw Error (InvalidState, "smint::process", "centre frequency mismatch\n"
                 "\t" + reference_filename
                      + " freq=" + tostring(centre_frequency) + "\n"
                 "\t" + archive->get_filename()
                      + " freq=" + tostring(archive->get_centre_frequency()) );

  if (ext->get_nchan() != channel_frequency.size())
    throw Error (InvalidState, "smint::process", "nchan mismatch\n"
                 "\t" + reference_filename
                      + " nchan=" + tostring(channel_frequency.size()) + "\n"
                 "\t" + archive->get_filename()
                      + " nchan=" + tostring(ext->get_nchan()) );

  for (unsigned i=0; i<ext->get_nchan(); i++)
    if (channel_frequency[i] != ext->get_centre_frequency(i) - centre_frequency)
      throw Error (InvalidState, "smint::process", "channel frequency mismatch\n"
                    "\t" + reference_filename +
                    " freq=" + tostring(centre_frequency+channel_frequency[i]) + "\n"
                    "\t" + archive->get_filename() +
                    " freq=" + tostring(ext->get_centre_frequency(i)) );
}

void smint::process (Pulsar::Archive* archive)
{
#if HAVE_SPLINTER
  if (previous_solution)
  {
    compare (archive);
    return;
  }
#endif

  data_filename = archive->get_filename();

  if (archive->get_nsubint() == 1)
  {
    if (!archive->get_dedispersed())
      cerr << "smint: WARNING data are not dedispersed" << endl;

    if (!archive->get_faraday_corrected())
      cerr << "smint: WARNING data are not corrected for Faraday rotation" << endl;

    cerr << "smint::process sub-integration" << endl;
    load_data (profile_data, archive->get_Integration(0));
    cerr << "smint::process loaded sub-integration" << endl;

    convert_epochs = false;

#if HAVE_PGPLOT
  if (plot_device != "")
    cpgopen (plot_device.c_str());
#endif

    for (unsigned ipol=0; ipol < profile_data.size(); ipol++)
    {
      string idx = tostring(profile_data[ipol].index);

      spline_filename = "profile_spline_" + idx;

      if (plot_device == "")
        plot_filename = "profile_fit_" + idx + ".eps/cps";

      // used in unload_row
      current_ipol = ipol;
      
      cerr << "smint::process fit ipol=" << ipol << endl;
      fit (profile_data[ipol]); 
    }

#if HAVE_SPLINTER
    if (!row_by_row)
      unload (archive->get_Integration(0), profile_data);
#endif

    string new_filename = replace_extension (archive->get_filename(), ".smar");
    archive->unload (new_filename);
    
#if HAVE_PGPLOT
  if (plot_device != "")
    cpgend ();
#endif

    cerr << "smint::process sub-integration data fit" << endl;
  }

  FluxCalibratorExtension* fext = archive->get<FluxCalibratorExtension>();
  if (fext)
  {
    if (fcal_data.size() == 0)
    {
      
      /*
	smooth either S_cal or scale for each of the receptor parameters 

	See FluxCalibratorExtension::get_Estimate index calculations.
      */

      unsigned nreceptor = fext->get_nreceptor ();
      cerr << "smint: fluxcal nreceptor=" << nreceptor << endl;

      unsigned istart = nreceptor;
      
      if (use_native_scale && fext->has_scale())
      {
	cerr << "smint: fluxcal native scale" << endl;
	istart += nreceptor;
      }

      unsigned iend = istart + nreceptor - 1;

      cerr << "smint: start=" << istart << " end=" << iend << endl;
      
      add_if_has_data (fcal_data, fext, istart, iend);
      set_reference (archive, fext);
    }
    else
    {
      check_reference (archive, fext);
    }

    MJD epoch = fext->get_epoch();
    load_data (fcal_data, fext, epoch);

  }

  PolnCalibratorExtension* ext = archive->get<PolnCalibratorExtension>();
  if (ext)
  {
    if (pcal_data.size() == 0)
    {
      /* smooth only receptor parameters 3, 4, 5, 6
         and ignore the backend (gain and phase) */

      add_if_has_data (pcal_data, ext, 3, 6);
      set_reference (archive, ext);
    }
    else
    {
      check_reference (archive, ext);
    }

    MJD epoch = ext->get_epoch();

    load_data (pcal_data, ext, epoch);

    Reference::To< CalibratorStokes > cal;
    cal = archive->get<CalibratorStokes>();
    if (cal)
    {
      if (cal_stokes_data.size() == 0)
      {
        // smooth Q=0, U=1, and V=2
        add_if_has_data (cal_stokes_data, cal.get(), 0, 2);
      }
    
      load_data (cal_stokes_data, cal.get(), epoch);
    }
  }

  input_filenames.push_back( archive->get_filename() );
}

void smint::load_data (vector<set>& data, Integration* subint)
{
  const unsigned nchan = subint->get_nchan();
  const unsigned nbin = subint->get_nbin();
  const unsigned npol = subint->get_npol();

  cerr << "smint::load Integration npol=" << npol << " nchan=" << nchan 
       << " nbin=" << nbin << endl;

  data.resize (npol);

  vector< vector< Estimate<double> > > mean;
  vector< vector< double > > variance;

  subint->baseline_stats (&mean, &variance);

  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    data[ipol].table.resize( nbin );
    data[ipol].index = ipol;

    for (unsigned ibin=0; ibin<nbin; ibin++)
    {
      std::vector< double >& data_x = data[ipol].table[ibin].freq;
      std::vector< Estimate<double> >& data_y = data[ipol].table[ibin].data;

      data[ipol].table[ibin].x0 = ibin;

      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
        if (subint->get_weight(ichan) == 0.0)
          continue;

        data_x.push_back( subint->get_centre_frequency(ichan) );

        double amp = subint->get_Profile(ipol,ichan)->get_amps()[ibin];
        double var = variance.at(ipol).at(ichan);

        data_y.push_back( Estimate<double>(amp,var) );
      }
    }
  }

  current_subint = subint;
}


#if HAVE_SPLINTER

void smint::unload (Integration* subint, vector<set>& data)
{
  const unsigned nchan = subint->get_nchan();
  const unsigned nbin = subint->get_nbin();
  const unsigned npol = subint->get_npol();

  assert (data.size() == npol);
  assert (data[0].table.size() == nbin);
  assert (data[0].table[0].freq.size() <= nchan);
  
  cerr << "smint::unload Integration npol=" << npol << " nchan=" << nchan 
       << " nbin=" << nbin << endl;

  for (unsigned ipol=0; ipol<npol; ipol++)
  {
    assert (data[ipol].index == ipol);

    for (unsigned ibin=0; ibin<nbin; ibin++)
    {
      assert (data[ipol].table[ibin].x0 == ibin);

      unload (subint, data[ipol], ipol, ibin);
    }
  }
}

void smint::unload (Integration* subint, set& data, unsigned ipol, unsigned ibin)
{
  assert (ibin < data.table.size());

  const unsigned nchan = subint->get_nchan();

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (subint->get_weight(ichan) == 0.0)
      subint->set_weight(ichan, 1.0);

    unsigned nrow = data.table.size();
    
    double xval = subint->get_centre_frequency(ichan);
    double yval = 0.0;
    
    if (nrow == 1 || row_by_row)
      yval = data.table[ibin].spline1d.evaluate (xval);
    else
    {
      pair<double,double> coord (data.table[ibin].x0, xval);
      yval = data.spline2d.evaluate ( coord );
    }

    float* amps = subint->get_Profile(ipol,ichan)->get_amps();
    amps[ibin] = yval;
  }
}

#endif

void smint::fit (set& dataset)
{
  vector<row>& table = dataset.table;

  if (table.size() == 0)
    throw Error (InvalidState, "smint::fit", "empty table");

  if (find_median_nfree)
  {
    vector<double> nfree (table.size());
    cerr << "smint::fit computing " << table.size() << " nfree values" << endl;
    for (unsigned irow=0; irow < table.size(); irow++)
    {
      SmoothingSpline spline;
      if (minimize_tmse)
	spline.set_msre (1.0);

      spline.fit (table[irow].freq, table[irow].data);
      nfree[irow] = spline.get_fit_effective_nfree ();
    }

    median_nfree = median (nfree);
    cerr << "median effective nfree = " << median_nfree << endl;
  }
  
  if (table.size() == 1 || row_by_row)
  {
    cerr << "smint::fit performing " << table.size() << " 1-D fits" << endl;
    for (unsigned irow=0; irow < table.size(); irow++)
    {
      if (use_smoothing_spline())
      {
	cerr << "smint::fit 1-D pspline to "
	     << table[irow].freq.size() << " points" << endl;

        data_filename = dataset.table[irow].filename;

	fit_pspline (dataset.table[irow].spline1d,
		     table[irow].freq, table[irow].data);
      }
      else
      {
	fit_polynomial (table[irow].freq, table[irow].data);
      }
      
      if (row_by_row)
	unload_row (dataset, irow);
    }

#if HAVE_PGPLOT
    if (row_by_row)
      cpgend ();
#endif
    
  }
  else
  {
    if (!use_smoothing_spline())
      throw Error (InvalidState, "smint::fit",
                   "2-D smoothing with polynomials not implemented");

#if HAVE_SPLINTER
    cerr << "smint::fit 2-D pspline" << endl;
    fit_pspline (dataset.spline2d, table);
#else
    throw Error (InvalidState, "smint::fit",
                 "2-D smoothing requires SPLINTER library");
#endif
  }
}

void smint::unload_row (set& dataset, unsigned irow)
{
  if (&dataset != &(profile_data[current_ipol]))
  {
    cerr << "unload row-by-row currently implemented only for pulse profile" << endl;
    return;
  }

#if HAVE_SPLINTER

  vector<row>& table = dataset.table;
  unsigned ibin = irow;  

  if (use_smoothing_spline())
  {
    cerr << "smint::unload_row ibin=" << ibin
	 << " nchan=" << table[irow].freq.size()  << endl;

    unload (current_subint, dataset, current_ipol, ibin);
  }
  else
#endif
  {
    cerr << "smint::unload_row not implemented for polynomial fits" << endl;
    // fit_polynomial (table[irow].freq, table[irow].data);
  }
}


void smint::unload (const string& filename, const vector<row>& table)
{
  cerr << "smint::unload filename=" << filename << endl;
  
  ofstream out (filename.c_str());
  for (unsigned irow=0; irow < table.size(); irow++)
  {
    string epoch = table[irow].epoch.printdays(4);
    for (unsigned idat=0; idat < table[irow].freq.size(); idat++)
    {
      out << epoch << " " << table[irow].freq[idat] 
          << " " << table[irow].data[idat].val
          << " " << sqrt(table[irow].data[idat].var) << endl;
    }
    out << endl;
  }
}

template<class Extension>
void zero (Extension* ext, unsigned iparam)
{
  unsigned nchan = ext->get_nchan();

  Estimate<float> zero (0.0);
  
  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!ext->get_valid (ichan))
      continue;
    
    ext->set_Estimate (iparam, ichan, zero);
  }
}

		    
template<class Extension>
void smint::unload (Extension* ext, vector<set>& data, unsigned ifile)
{
  unsigned nchan = ext->get_nchan();

  assert (nchan == channel_frequency.size());

  for (unsigned i=0; i < data.size(); i++)
  {
    unsigned iparam = data[i].index;
    unsigned nrow = data[i].table.size();

    assert (nrow == input_filenames.size());

    double xval=0, yval=0;

    if (bootstrap_uncertainty && (nrow == 1 || row_by_row))
    {
      interpolate = false;
      
      BootstrapUncertainty bootstrap;
      cerr << "smint: computing error bars (bootstrap replacement)" << endl;
      bootstrap.set_spline (& data[i].table[ifile].spline1d);
      bootstrap.get_uncertainty (data[i].table[ifile].freq,
				 data[i].table[ifile].data);
    }

    unsigned data_index = 0;
    
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (interpolate)
	ext->set_valid (ichan, true);

      if (!ext->get_valid (ichan))
	continue;
      
      xval = channel_frequency[ichan];
      Estimate<float> val;
      
      if (nrow == 1 || row_by_row)
      {
	if (bootstrap_uncertainty)
	{
	  assert (xval == data[i].table[ifile].freq[data_index]);
	  val = data[i].table[ifile].data[data_index];
	  data_index ++;
	}
	else
	{
	  yval = data[i].table[ifile].spline1d.evaluate (xval);
	  val = Estimate<float> (yval, fabs(yval)*1e-4);
	}
      }
      else
      {	
#if HAVE_SPLINTER
        pair<double,double> coord (data[i].table[ifile].x0, xval);
        yval = data[i].spline2d.evaluate ( coord );
	val = Estimate<float> (yval, fabs(yval)*1e-4);
#endif
      }

      ext->set_Estimate (iparam, ichan, val);
    }
  }
}

template <class T>
void delete_extension (Archive* archive)
{
  T* ext = archive->get<T>();
  if (ext)
    delete ext;
}

void smint::prepare_solution (Archive* archive)
{
  result = archive->getadd <CalibrationInterpolatorExtension> ();

  auto pcext = archive->get<PolnCalibratorExtension> ();
  if (pcext)
  {  
    result->set_type( pcext->get_type() );
    delete pcext;
  }

  auto csext = archive->get<CalibratorStokes> ();
  if (csext)
  {
    result->set_coupling_point( csext->get_coupling_point() );
    delete csext;
  }
  
  auto fcext = archive->get<FluxCalibratorExtension> ();
  if (fcext)
  {
    result->set_type( new CalibratorTypes::Flux );
    result->set_nreceptor( fcext->get_nreceptor() );
    result->set_native_scale( use_native_scale && fcext->has_scale() );
    delete fcext;
  }
}

void smint::finalize ()
{
#if HAVE_SPLINTER
  if (previous_solution)
    return;
#endif

  cerr << "smint::finalize" << endl;

  Reference::To<Archive> solution;

  unload_smoothed = (input_filenames.size() == 1 || row_by_row);
  unload_solution = !unload_smoothed;
  
  if (unload_solution)
  {
    cerr << "smint::finalize reference filename=" << reference_filename << endl;
    solution = Archive::load (reference_filename);

    smint::prepare_solution (solution);
  }
  
  for (unsigned i=0; i < pcal_data.size(); i++)
  {
    string filename = "pcal_data_" + tostring(pcal_data[i].index) + ".txt";
    unload (filename, pcal_data[i].table);
  }

  for (unsigned i=0; i < cal_stokes_data.size(); i++)
  {
    string idx = tostring(cal_stokes_data[i].index);
    string filename = "cal_stokes_data_" + idx + ".txt";
    unload (filename, cal_stokes_data[i].table);
  }

  for (unsigned i=0; i < fcal_data.size(); i++)
  {
    string filename = "fcal_data_" + tostring(fcal_data[i].index) + ".txt";
    unload (filename, fcal_data[i].table);
  }

#if HAVE_PGPLOT
  if (plot_device != "")
    cpgopen (plot_device.c_str());
#endif

  convert_epochs = true;
  model_code = CalibrationInterpolatorExtension::Parameter::FrontendParameter;

  for (unsigned i=0; i < pcal_data.size(); i++)
  {
    model_index = pcal_data[i].index;
    
    cerr << "smint: fitting PolnCalibrator "
            "iparam=" << model_index << endl;

    string idx = tostring(model_index);

    spline_filename = "pcal_spline_" + idx;

    if (plot_device == "")
      plot_filename = "pcal_fit_" + idx + ".eps/cps";

    fit (pcal_data[i]);
  }

  model_code = CalibrationInterpolatorExtension::Parameter::CalibratorStokesParameter;
      
  for (unsigned i=0; i < cal_stokes_data.size(); i++)
  {
    model_index = cal_stokes_data[i].index;
    
    cerr << "smint: fitting CalibratorStokes iparam=" << model_index << endl;

    string idx = tostring(model_index);
    spline_filename = "cal_stokes_spline_" + idx;

    if (plot_device == "")
      plot_filename = "cal_stokes_fit_" + idx + ".eps/cps";

    fit (cal_stokes_data[i]);
  }

  model_code = CalibrationInterpolatorExtension::Parameter::FluxCalibratorParameter;

  for (unsigned i=0; i < fcal_data.size(); i++)
  {
    model_index = fcal_data[i].index;
    
    cerr << "smint: fitting FluxCalibrator "
            "iparam=" << model_index << endl;

    string idx = tostring(model_index);

    spline_filename = "fcal_spline_" + idx;

    if (plot_device == "")
      plot_filename = "fcal_fit_" + idx + ".eps/cps";

    fit (fcal_data[i]);
  }

#if HAVE_PGPLOT
  if (plot_device != "")
    cpgend ();
#endif

  if (solution)
    solution->unload ("smint.fits");
  
  if (!unload_smoothed)
    return;
      
  cerr << "smint::finalize unloading smoothed solutions" << endl;
  
  for (unsigned ifile=0; ifile < input_filenames.size(); ifile++)
  {
    string filename = input_filenames[ifile];

    Reference::To<Archive> archive = Archive::load(filename);

    if (pcal_data.size())
    {
      PolnCalibratorExtension* ext;
      ext = archive->get<PolnCalibratorExtension>();
      if (!ext)
        throw Error (InvalidState, "smint", 
                     filename + " does not have PolnCalibratorExtension");

      ext->set_has_covariance (false);

      zero (ext, 0);
      zero (ext, 1);
      zero (ext, 2);
      
      unload (ext, pcal_data, ifile);
    }

    if (cal_stokes_data.size())
    {
      CalibratorStokes* ext;
      ext = archive->get< CalibratorStokes >();
      if (!ext)
        throw Error (InvalidState, "smint",
                     filename + " does not have CalibratorStokes");

      unload (ext, cal_stokes_data, ifile);
    }

    if (fcal_data.size())
    {
      FluxCalibratorExtension* ext;
      ext = archive->get<FluxCalibratorExtension>();
      if (!ext)
        throw Error (InvalidState, "smint",
                     filename + " does not have FluxCalibratorExtension");

      unload (ext, fcal_data, ifile);
    }

    string new_filename = replace_extension (filename, ".smint");
    archive->unload (new_filename);
  }

}


#ifdef HAVE_PGPLOT

template<class Spline>
void smint::setup_and_plot (Spline& model,
			    const vector< double >& data_x,
			    const vector< Estimate<double> >& data_y)
{
  cerr << "setup_and_plot filename = " << plot_filename << endl;
  
  if (plot_filename != "")
    cpgopen (plot_filename.c_str());

  if (row_by_row)
    plot_filename = "";
  
  plot_npts = 200;

  cpgpage();
  plot_data (data_x, data_y, data_filename);

  cpgsci(2);

  plot_xmin = *min_element(data_x.begin(), data_x.end());
  plot_xmax = *max_element(data_x.begin(), data_x.end());

  plot_model (model, plot_npts, plot_xmin, plot_xmax);

  if (plot_filename != "")
    cpgend ();
}

#endif

void smint::fit_pspline (SmoothingSpline& spline,
			 vector< double >& data_x,
			 vector< Estimate<double> >& data_y)
{
  if (interquartile_range)
    iteratively_remove_outliers (spline, data_x, data_y);
  
  if (pspline_alpha)
    spline.set_smoothing (pspline_alpha);

  if (find_median_nfree)
    spline.set_effective_nfree (median_nfree);

  if (minimize_tmse)
    spline.set_msre (1.0);

  if (cross_validate)
  {
    cross_validated_smoothing->set_spline (&spline);
    cross_validated_smoothing->fit (data_x, data_y);
  }
  else
  {
    spline.fit (data_x, data_y);
    double nfree = spline.get_fit_effective_nfree ();
    cerr << "smint::fit_pspline effective nfree=" << nfree << endl;
  }
  
#ifdef HAVE_PGPLOT
  setup_and_plot (spline, data_x, data_y);
#endif

}

template<class Container>
void erase (Container& container, unsigned index)
{
  container.erase (container.begin()+index);
}

void smint::iteratively_remove_outliers (SmoothingSpline& spline,
					 vector< double >& data_x,
					 vector< Estimate<double> >& data_y)
{
  bool done = false;

  if (outlier_smoothing > 0)
    spline.set_effective_nfree (data_x.size() / outlier_smoothing);
  else
    spline.set_minimize_gcv (true);
  
  while (!done)
  {
    spline.fit (data_x, data_y);    
    done = remove_outliers (spline, data_x, data_y) == 0;
  }

  spline.set_minimize_gcv (true);
}


unsigned smint::remove_outliers (SmoothingSpline& spline,
				 vector< double >& data_x,
				 vector< Estimate<double> >& data_y)
{
  unsigned npts = data_x.size();

  vector<double> normalized_residual (npts);

  unsigned ival = 0;
  
  for (unsigned i=0; i<npts; i++)
  {
    if (data_y[i].var == 0.0)
      continue;
    
    double yval = spline.evaluate (data_x[i]);
    normalized_residual[ival] = (data_y[i].val - yval) / sqrt(data_y[i].var);
    ival ++;
  }

  normalized_residual.resize (ival);
  
  double Q1, Q2, Q3;
  Q1_Q2_Q3 (normalized_residual, Q1, Q2, Q3);

  double IQR = Q3 - Q1;
  double lower_threshold = Q1 - interquartile_range * IQR;
  double upper_threshold = Q3 + interquartile_range * IQR;

  unsigned ipt = 0;
  unsigned removed = 0;

  while (ipt < data_x.size())
  {
    if (data_y[ipt].var == 0.0)
      continue;
    
    double yval = spline.evaluate (data_x[ipt]);
    double nres = (data_y[ipt].val - yval) / sqrt(data_y[ipt].var);

    if ( (nres < lower_threshold) ||
	 (nres > upper_threshold) )
    {
      erase (data_x, ipt);
      erase (data_y, ipt);
      removed ++;
    }
    else
      ipt++;
  }

  cerr << "smint::remove_outliers removed " << removed << endl;
  return removed;
}

#if HAVE_SPLINTER

double chisq (SplineSmooth2D& spline,
	      const vector< pair<double,double> >& data_x,
	      const vector< Estimate<double> >& data_y)
{
  unsigned ndat = data_x.size ();
  assert (ndat == data_y.size ());

  double total_chisq = 0.0;
  
  for (unsigned i=0; i<ndat; i++)
  {
    if (data_y[i].var > 0)
    {
      double diff = spline.evaluate ( data_x[i] ) - data_y[i].val;
      total_chisq += diff*diff/data_y[i].var;
    }
  }

  return total_chisq;
}
	      
void smint::fit_pspline (SplineSmooth2D& spline, vector<row>& table)
{
  if (table.size() == 0)
    throw Error (InvalidParam, "smint::fit_pspline",
		 "no data");

  unsigned nchan = table[0].freq.size();  

  for (unsigned irow = 0; irow < table.size(); irow++)
    {
      assert (table[irow].data.size() == nchan);
    }
  
  unsigned min_chan = 0;
  unsigned max_chan = 0;

  // searching for first data
  bool search_for_min = true;

  double xmin = 0;
  double xmax = 0;

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    bool have_data = false;
    double freq = table[0].freq[ichan];
    for (unsigned irow = 0; irow < table.size(); irow++)
    {
      assert (table[irow].freq[ichan] == freq);
      
      if (table[irow].data[ichan].var > 0)
      {
	have_data = true;
	break;
      }
    }

    if (have_data)
    {
      if (search_for_min)
      {
	min_chan = ichan;
	search_for_min = false;
      }
      else
      {
	max_chan = ichan;
      }

      xmin = std::min (xmin, freq);
      xmax = std::max (xmax, freq);
    }
  }

#if _DEBUG
  cerr << "channel bounds min=" << min_chan << " max=" << max_chan << endl;
  cerr << "frequency xmin=" << xmin << " xmax=" << xmax << endl;
#endif

  if (result)
  {
    result->set_nchan_input (nchan);
    result->set_reference_frequency (centre_frequency);
    
    double freq = result->get_minimum_frequency();
    if (freq != 0.0)
      assert (freq == xmin+centre_frequency);
    else
      result->set_minimum_frequency (xmin+centre_frequency);

    freq = result->get_maximum_frequency();
    if (freq != 0.0)
      assert (freq == xmax+centre_frequency);
    else
      result->set_maximum_frequency (xmax+centre_frequency);
  }
  
  if (convert_epochs)
  {
    min_epoch = table.front().epoch;
    max_epoch = table.front().epoch;

    for (unsigned irow = 1; irow < table.size(); irow++)
    {
      if (table[irow].epoch > max_epoch)
        max_epoch = table[irow].epoch;
      if (table[irow].epoch < min_epoch)
        min_epoch = table[irow].epoch;
    }
  
    double x0_span = (max_epoch - min_epoch).in_days();
  
    cerr << "smint::fit_pspline data span " << x0_span << " days" << endl;
  
    reference_epoch = min_epoch + MJD(0.5 * x0_span);
  
    cerr << "smint::fit_pspline min=" << min_epoch
	 << " ref=" << reference_epoch << endl;

    convert_epochs = false;

    if (result)
    {
      result->set_nsub_input (table.size());
      result->set_reference_epoch (reference_epoch);
      result->set_minimum_epoch (min_epoch);
      result->set_maximum_epoch (max_epoch);
    }
  }

  double x0_span = (max_epoch - min_epoch).in_days();
    
  spline.set_alpha (pspline_alpha);

  vector< pair<double,double> > data_x;
  vector< Estimate<double> > data_y;

  unsigned ndat_input = 0;
  
  for (unsigned irow = 0; irow < table.size(); irow++)
  {
    table[irow].x0 = (table[irow].epoch - reference_epoch).in_days();

    double x0 = table[irow].x0;

    for (unsigned ichan=min_chan; ichan <= max_chan; ichan++)
    {
      data_x.push_back ( pair<double,double>( x0, table[irow].freq[ichan] ) );
      data_y.push_back ( table[irow].data[ichan] );

      if (table[irow].data[ichan].var > 0)
	ndat_input ++;
    }
  }

  CalibrationInterpolatorExtension::Parameter* param = 0;
  if (result)
  {
    param = new CalibrationInterpolatorExtension::Parameter;
    param->code = model_code;
    param->iparam = model_index;
    param->ndat_input = ndat_input;
    result->add_parameter (param);
  }
  
  if (cross_validate)
  {
    if (spline_filename != "")
    {
      string filename = spline_filename + ".gof";
      cross_validated_smoothing_2D->set_gof_filename (filename);
    }

    cross_validated_smoothing_2D->set_spline (&spline);
    cross_validated_smoothing_2D->fit (data_x, data_y);

    if (param)
    {
      param->ndat_flagged_before
	= cross_validated_smoothing_2D->get_nflagged_iqr ();

      param->ndat_flagged_after
	= cross_validated_smoothing_2D->get_nflagged_gof ();
    }
  }
  else
  {
    spline.fit (data_x, data_y);
  }

  if (param)
  {
    param->log10_smoothing_factor = log10( spline.get_alpha() );
    param->total_chi_squared = chisq (spline, data_x, data_y);

    const char* filename = "smint.tmp";
    spline.unload (filename);

    ifstream in (filename);
    in >> param->interpolator;

    // cerr << "interpolator nchar=" << param->interpolator.size() << endl;
  }
  
  if (spline_filename != "")
  {
    string filename = spline_filename + ".out";
    spline.unload (filename);
    cerr << "spline written to " << filename << endl;
    spline.load (filename);
    cerr << "spline reloaded from " << filename << endl;

    plot_npts = 200;

    filename = spline_filename + ".txt";
    cerr << "writing " << plot_npts << "x" << plot_npts << " grid to " 
         << filename << endl;

    double x1del = (xmax-xmin)/plot_npts;
    double x0del = x0_span/plot_npts;
    double x0min = -0.5*x0_span;

    ofstream out (filename.c_str());
    for (unsigned i=0; i<plot_npts; i++)
    {
      double x0 = x0min + x0del * double(i);

      for (unsigned j=0; j<plot_npts; j++)
      {
        double x1 = xmin + x1del * double(j);
  
        double y = spline.evaluate ( pair<double,double>(x0,x1) );
  
        out << x0 << " " << x1 << " " << y << endl;
      }
  
      out << endl;
    }

    cerr << "grid written to " << filename << endl;
  }

#ifdef HAVE_PGPLOT

  if (plot_filename != "")
    cpgopen (plot_filename.c_str());

  for (unsigned irow = 0; irow < table.size(); irow++)
  {
    cpgpage();
    plot_data (table[irow].freq, table[irow].data, table[irow].filename);

    cpgsci(2); 
    double x0 = table[irow].x0;

    plot_model (spline, x0, plot_npts, xmin, xmax);
  }

  if (plot_filename != "")
    cpgend ();

#endif

}

#endif // HAVE_SPLINTER

void smint::fit_polynomial (const vector< double >& data_x,
			    const vector< Estimate<double> >& data_y)
{
  if (freq_order == 0)
    throw Error (InvalidState, "smint::fit_polynomial",
                 "order of polynomial along radio frequency == 0");

  Reference::To< MEAL::Scalar > scalar = new MEAL::Polynomial(freq_order);

  MEAL::Axis<double> argument;
  scalar->set_argument (0, &argument);

  std::vector< MEAL::Axis<double>::Value > axis_x; // (data_x.size());

  for (unsigned i=0; i<data_x.size(); i++)
    axis_x.push_back( argument.get_Value( data_x[i] ) );

#ifdef HAVE_PGPLOT

  std::pair<MEAL::Axis<double>*, MEAL::Scalar*> model;
  model.first = &argument;
  model.second = scalar;

  setup_and_plot (model, data_x, data_y);

#endif

  MEAL::LevenbergMarquardt<double> fit;

  float chisq = fit.init (axis_x, data_y, *scalar);
  cerr << "initial chisq = " << chisq << endl;

  unsigned iter = 1;
  unsigned not_improving = 0;

  while (not_improving < 25)
  {
    cerr << "iteration " << iter << endl;
    float nchisq = fit.iter (axis_x, data_y, *scalar);
    cerr << "     chisq = " << nchisq << endl;

    if (nchisq < chisq)
    {
      float diff_chisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diff_chisq/chisq < threshold && diff_chisq > 0)
      {
        cerr << "no big diff in chisq = " << diff_chisq << endl;
        break;
      }
    }
    else
      not_improving ++;

    iter ++;
  }

  double free_parms = axis_x.size() + scalar->get_nparam();

  cerr << "Chi-squared = " << chisq << " / " << free_parms << " = "
       << chisq / free_parms << endl;

#ifdef HAVE_PGPLOT
  cpgsci(3);
  plot_model (model, plot_npts, plot_xmin, plot_xmax);
#endif

  string model_text;
  scalar->print (model_text);
  cout << "ANSWER:\n" << model_text << endl;
}

#ifdef HAVE_PGPLOT

void smint::plot_data (const vector< double >& data_x,
                       const vector< Estimate<double> >& data_y,
                       const string& filename)
{
  //
  // Plot the data
  //

  EstimatePlotter plot;
  plot.add_plot (data_x, data_y);

  cpgsch (1.5);
  cpgsci (1);
  cpgslw (1);

  plot.plot();

  cpgbox ("bcinst", 0,0, "bcinst", 0,0);
  cpglab ("Frequency Offset (MHz)", "Data", filename.c_str());
}

void smint::plot_model (std::pair<MEAL::Axis<double>*, MEAL::Scalar*>& model,
                        unsigned npts, double xmin, double xmax)
{
  MEAL::Axis<double>& argument = *(model.first);
  MEAL::Scalar* scalar = model.second;
    
  double xdel = (xmax-xmin)/(npts-1);

  for (unsigned i=0; i<npts; i++) {

    double x = xmin + xdel * double(i);

    argument.set_value (x);

    double y = scalar->evaluate ();

    if (i==0)
      cpgmove (x,y);
    else
      cpgdraw (x,y);
  }
}

void smint::plot_model (SmoothingSpline& spline,
                        unsigned npts, double xmin, double xmax)
{
  cerr << "plot_model SplineSmooth1D" << endl;
  
  double xdel = (xmax-xmin)/(npts-1);
  for (unsigned i=0; i<npts; i++)
  {
    double xval = xmin + xdel * double(i);
    double yval = spline.evaluate (xval);

    if (i==0)
      cpgmove (xval,yval);
    else
      cpgdraw (xval,yval);
  }

}

#if HAVE_SPLINTER

void smint::plot_model (SplineSmooth2D& spline, double x0,
                        unsigned npts, double xmin, double xmax)
{
  double xdel = (xmax-xmin)/(npts-1);
  for (unsigned i=0; i<npts; i++)
  {
    double xval = xmin + xdel * double(i);
    double yval = spline.evaluate ( pair<double,double> (x0,xval) );

    if (i==0)
      cpgmove (xval,yval);
    else
      cpgdraw (xval,yval);
  }
}

#endif  // HAVE_SPLINTER

#endif  // HAVE_PGPLOT

#if HAVE_SPLINTER

void smint::load_previous_solution (const string& filename) try
{
  previous_solution = Pulsar::Archive::load (filename);
  previous_interpolator = new CalibrationInterpolator (previous_solution);
}
catch (Error& error)
{
  throw error += "smint::load_previous_solution";
}

template <class Extension>
void smint::compare (const Archive* arch, 
                     CalibrationInterpolatorExtension::Parameter::Type type)
{
  const Extension* ext = arch->get<Extension>();
  if (!ext)
    throw Error (InvalidState, "smint::compare", 
                 arch->get_filename()
                 + " does not have " + tostring(type));

  this->update (ext);

  const Extension* prev = previous_solution->get<Extension>();
  if (!prev)
    throw Error (InvalidState, "smint::compare", 
                 previous_solution->get_filename()
                 + " does not have " + tostring(type));

  cerr << "compare " << type << " of " << arch->get_filename () << " with "
       << previous_solution->get_filename () << endl;

  const CalibrationInterpolatorExtension* interpolator
    = previous_interpolator->get_extension();

  unsigned nparam = interpolator->get_nparam ();

  for (unsigned iparam=0; iparam < nparam; iparam++)
  {
    const CalibrationInterpolatorExtension::Parameter* param 
      = interpolator->get_parameter (iparam);

    if (param->get_code() == type)
    {
      double gof = compute_gof (ext, prev, param->get_iparam());
      cout << arch->get_filename() << " " << type << " " 
           << param->get_iparam() << " " << gof << endl;
    }
  }
}

void smint::compare (const Archive* arch)
{
  if (previous_interpolator->get_type()->is_a<CalibratorTypes::Flux>())
  {
    compare<FluxCalibratorExtension> (arch, 
    CalibrationInterpolatorExtension::Parameter::FluxCalibratorParameter);
  }
  else
  {
    compare<PolnCalibratorExtension> (arch, 
    CalibrationInterpolatorExtension::Parameter::FrontendParameter);

    compare<CalibratorStokes> (arch, 
    CalibrationInterpolatorExtension::Parameter::CalibratorStokesParameter);
  }
}

template<class Container>
double smint::compute_gof (const Container* ext, const Container* prev,
                           unsigned iparam)
{
  unsigned nchan = ext->get_nchan();
  double chisq = 0.0;
  unsigned count = 0;

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!ext->get_valid(ichan))
      continue;

    Estimate<float> dat = ext->get_Estimate (iparam, ichan);
    Estimate<float> pred = prev->get_Estimate (iparam, ichan);

    double diff = dat.get_value() - pred.get_value();
    chisq += diff * diff / dat.get_variance();
    count ++;
  }

  return chisq / count;
}

#endif

/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  smint program;
  return program.main (argc, argv);
}

