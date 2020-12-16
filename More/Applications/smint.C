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
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Profile.h"

#undef HAVE_PGPLOT

#ifdef HAVE_PGPLOT
#include <cpgplot.h>
#include "EstimatePlotter.h"
#endif

#ifdef HAVE_SPLINTER
#include "Pulsar/SplineSmooth.h"
#endif

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

#include "strutil.h"

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

protected:

  bool convert_epochs;

  void fit_polynomial (const vector< double >& data_x,
                       const vector< Estimate<double> >& data_y);

#if HAVE_PGPLOT
  void plot_data (const vector< double >& data_x,
                  const vector< Estimate<double> >& data_y);

  void plot_model (MEAL::Axis<double>& argument,
                   MEAL::Scalar* scalar,
                   unsigned npts, double xmin, double xmax);
#endif

  //! Add command line options
  void add_options (CommandLine::Menu&);

  double pspline_alpha;

  unsigned freq_order;
  unsigned time_order;
  float threshold;

  string plot_device;

  // a spectrum of estimates observed at a given epoch
  class row
  {
  public:
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
    SplineSmooth1D spline1d;
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

  // load profile data from Integration
  void load (vector<set>& data, Integration* subint);

  // load data from a container into one or more set::table
  template<class Container>
  void load (vector<set>& data, Container* ext, const MJD& epoch);

  // add parameters, from istart to iend inclusive, only if they have been measured
  template<class Container>
  void add_if_has_data (vector<set>& data, Container* ext,
                        unsigned istart, unsigned iend);

  template<typename Container>
  void set_reference (Pulsar::Archive* archive, Container* ext);

  template<typename Container>
  void check_reference (Pulsar::Archive* archive, Container* ext);

#if HAVE_SPLINTER
  
  // unload smoothed model values into container
  template<class Container>
  void unload (Container* ext, vector<set>& data, unsigned ifile);

  void unload (Integration* subint, vector<set>& data);
  void unload (Integration* subint, set& data, unsigned ipol, unsigned ibin);

#endif

  // unload one row of smoothed model values into container
  void unload_row (set& dataset, unsigned irow);

  // unload text to file
  void unload (const string& filename, const vector<row>& table);

  string spline_filename;
  string plot_filename;

  // fit 2-D data using a bunch of 1-D fits
  bool row_by_row;

  // the current sub-integration when fitting profile data
  Pulsar::Integration* current_subint;

  // the current polarization when fitting profile data
  unsigned current_ipol;
  
  void fit (set&);

#if HAVE_SPLINTER

  void fit_pspline (SplineSmooth1D& spline,
                    const vector< double >& data_x,
                    const vector< Estimate<double> >& data_y);

  void fit_pspline (SplineSmooth2D& spline, vector<row>& table);

#if HAVE_PGPLOT
  void plot_model (SplineSmooth1D& spline,
                   unsigned npts, double xmin, double xmax);

  void plot_model (SplineSmooth2D& spline, double x0,
                   unsigned npts, double xmin, double xmax);

#endif

#endif

};


/*!


*/

smint::smint ()
  : Application ("smint", "smooth and interpolate")
{
  add( new Pulsar::StandardOptions );

  freq_order = time_order = 0;
  threshold = 0.001;
  pspline_alpha = 0.0;
  row_by_row = false;

  current_subint = 0;
}


/*!


*/

void smint::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "Polynomial fitting options:");

  arg = menu.add (freq_order, 'f', "int");
  arg->set_help ("order of polynomial along radio frequency");

  arg = menu.add (time_order, "t", "int");
  arg->set_help ("order of polynomial along time");

  arg = menu.add (row_by_row, "1");
  arg->set_help ("fit 2-D data using series of 1-D fits");
  
#if HAVE_SPLINTER
  // add a blank line and a header to the output of -h
  menu.add ("\n" "Penalized spline (p-spline) fitting options:");

  arg = menu.add (pspline_alpha, 'p', "double");
  arg->set_help ("p-spline smoothing factor 'alpha'");
#endif

#if HAVE_PGPLOT
  // add a blank line and a header to the output of -h
  menu.add ("\n" "Plotting options:");

  arg = menu.add (plot_device, 'D', "dev");
  arg->set_help ("print all plots to a single device");
#endif

}


/*!


*/

template<class Container>
void smint::load (vector<set>& data, Container* ext, const MJD& epoch)
{
  for (unsigned i=0; i<data.size(); i++)
  {
    data[i].table.resize( data[i].table.size() + 1 );

    std::vector< double >& data_x = data[i].table.back().freq;
    std::vector< Estimate<double> >& data_y = data[i].table.back().data;

    data[i].table.back().epoch = epoch;

    unsigned nchan = ext->get_nchan();
    unsigned iparam = data[i].index;

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (!ext->get_valid(ichan))
        continue;

      Estimate<float> val = ext->get_Estimate (iparam, ichan);

      if (val.var == 0.0)
      {
        cerr << "smint::load ignoring valid data in ichan=" << ichan << " with zero variance" << endl;
        continue;
      }

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
  if (archive->get_nsubint() == 1)
  {
    if (!archive->get_dedispersed())
      cerr << "smint: WARNING data are not dedispersed" << endl;

    if (!archive->get_faraday_corrected())
      cerr << "smint: WARNING data are not corrected for Faraday rotation" << endl;

    cerr << "smint::process sub-integration" << endl;
    load (profile_data, archive->get_Integration(0));
    cerr << "smint::process loaded sub-integration" << endl;

    convert_epochs = false;

#if HAVE_PGPLOT
  if (plot_device != "")
    cpgopen (plot_device.c_str());
#endif

    for (unsigned ipol=0; ipol < profile_data.size(); ipol++)
    {
      string idx = tostring(profile_data[ipol].index);

      spline_filename = "profile_spline_" + idx + ".txt";

      if (plot_device == "")
        plot_filename = "profile_fit_" + idx + ".eps/cps";

      // used in unload_row
      current_ipol = ipol;
      
      cerr << "smint::process fit ipol=" << ipol << endl;
      fit (profile_data[ipol]); 
    }

    if (!row_by_row)
      unload (archive->get_Integration(0), profile_data);

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
      // smooth only S_cal for each receptor parameters 

      add_if_has_data (fcal_data, fext, 2, 3);
      set_reference (archive, fext);
    }
    else
    {
      check_reference (archive, fext);
    }

    MJD epoch = fext->get_epoch();
    load (fcal_data, fext, epoch);

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

    load (pcal_data, ext, epoch);

    Reference::To< CalibratorStokes > cal;
    cal = archive->get<CalibratorStokes>();
    if (cal)
    {
      if (cal_stokes_data.size() == 0)
      {
        // smooth Q=0, U=1, and V=2
        add_if_has_data (cal_stokes_data, cal.get(), 0, 2);
      }
    
      load (cal_stokes_data, cal.get(), epoch);
    }
  }

  input_filenames.push_back( archive->get_filename() );
}

void smint::load (vector<set>& data, Integration* subint)
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
      yval = data.spline1d.evaluate (xval);
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

  if (table.size() == 1 || row_by_row)
  {
    cerr << "smint::fit performing " << table.size() << " 1-D fits" << endl;
    for (unsigned irow=0; irow < table.size(); irow++)
    {
#if HAVE_SPLINTER
      if (pspline_alpha)
	{
	  cerr << "smint::fit 1-D pspline to " << table[irow].freq.size() << " points" << endl;
	  fit_pspline (dataset.spline1d, table[irow].freq, table[irow].data);
	}
      else
#endif
	fit_polynomial (table[irow].freq, table[irow].data);

      if (row_by_row)
	unload_row (dataset, irow);
    }
  }
  else
  {
    if (!pspline_alpha)
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
    throw Error (InvalidState, "smint::unload_row",
		 "fitting row-by-row currently implemented only for pulse profile");
  
  vector<row>& table = dataset.table;
  unsigned ibin = irow;  

#if HAVE_SPLINTER
  if (pspline_alpha)
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

#if HAVE_SPLINTER

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

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      xval = channel_frequency[ichan];
      if (nrow == 1)
        yval = data[i].spline1d.evaluate (xval);
      else
      {
        pair<double,double> coord (data[i].table[ifile].x0, xval);
        yval = data[i].spline2d.evaluate ( coord );
      }

      ext->set_valid (ichan, true);
      Estimate<float> val (yval, fabs(yval)*1e-4);
      ext->set_Estimate (iparam, ichan, val);
    }
  }
}

#endif

void smint::finalize ()
{
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

  for (unsigned i=0; i < pcal_data.size(); i++)
  {
    cerr << "smint: fitting PolnCalibrator "
            "iparam=" << pcal_data[i].index << endl;

    string idx = tostring(pcal_data[i].index);

    spline_filename = "pcal_spline_" + idx + ".txt";

    if (plot_device == "")
      plot_filename = "pcal_fit_" + idx + ".eps/cps";

    convert_epochs = true;

    fit (pcal_data[i]);
  }

  for (unsigned i=0; i < cal_stokes_data.size(); i++)
  {
    cerr << "smint: fitting CalibratorStokes iparam=" << i << endl;

    string idx = tostring(cal_stokes_data[i].index);
    spline_filename = "cal_stokes_spline_" + idx + ".txt";

    if (plot_device == "")
      plot_filename = "cal_stokes_fit_" + idx + ".eps/cps";

    convert_epochs = true;

    fit (cal_stokes_data[i]);
  }

  for (unsigned i=0; i < fcal_data.size(); i++)
  {
    cerr << "smint: fitting FluxCalibrator "
            "iparam=" << fcal_data[i].index << endl;

    string idx = tostring(fcal_data[i].index);

    spline_filename = "fcal_spline_" + idx + ".txt";

    if (plot_device == "")
      plot_filename = "fcal_fit_" + idx + ".eps/cps";

    convert_epochs = true;

    fit (fcal_data[i]);
  }

#if HAVE_PGPLOT
  if (plot_device != "")
    cpgend ();
#endif

#if HAVE_SPLINTER

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

#endif

}

#if HAVE_SPLINTER

void smint::fit_pspline (SplineSmooth1D& spline,
                         const vector< double >& data_x,
                         const vector< Estimate<double> >& data_y)
{
  double xmin = data_x[0];
  double xmax = xmin;

  double ymin = data_y[0].val;
  double ymax = ymin;

  unsigned nchan = data_x.size();

  vector<double> slopes(nchan-1);
  vector<double> errors(nchan-1);

  for (unsigned ichan=1; ichan < nchan; ichan++)
  {
    xmin = min(xmin, data_x[ichan]);
    xmax = max(xmax, data_x[ichan]);

    ymin = min(ymin, data_y[ichan].val);
    ymax = max(ymax, data_y[ichan].val);

    Estimate<double> delta_y = data_y[ichan] - data_y[ichan-1];
    double delta_x = data_x[ichan] - data_x[ichan-1];
    slopes[ichan-1] = fabs (delta_y.val / (delta_x * delta_x * sqrt(delta_y.var)));
    errors[ichan-1] = sqrt(delta_y.var);
  }

  // compute the median deviation
  std::sort (slopes.begin(), slopes.end());
  std::sort (errors.begin(), errors.end());

  double scale = slopes[nchan/2] * errors[nchan/2]; // (xmax-xmin) / (ymax-ymin);

  double use_alpha = pspline_alpha / ( scale * scale );
  cerr << "1D: alpha=" << use_alpha << endl;

  spline.set_alpha (use_alpha);
  spline.set_data (data_x, data_y);

#ifdef HAVE_PGPLOT

  if (plot_filename != "")
    cpgopen (plot_filename.c_str());

  if (row_by_row)
    plot_filename = "";
  
  unsigned npts = 500;

  cpgpage();

  plot_data (data_x, data_y);

  cpgsci(2);
  cpgslw(3);

  plot_model (spline, npts, xmin, xmax);

  if (plot_filename != "")
    cpgend ();

#endif

}


void smint::fit_pspline (SplineSmooth2D& spline, vector<row>& table)
{ 
  spline.set_alpha (pspline_alpha);

  MJD mid_epoch;
  double x0_span = 0.0;

  if (convert_epochs)
  {
    MJD min_epoch = table.front().epoch;
    MJD max_epoch = table.front().epoch;

    for (unsigned irow = 1; irow < table.size(); irow++)
    {
      if (table[irow].epoch > max_epoch)
        max_epoch = table[irow].epoch;
      if (table[irow].epoch < min_epoch)
        min_epoch = table[irow].epoch;
    }
  
    x0_span = (max_epoch - min_epoch).in_days();
  
    cerr << "smint::fit_pspline data span " << x0_span << " days" << endl;
  
    mid_epoch = min_epoch + MJD(0.5 * x0_span);
  
    cerr << "smint::fit_pspline min=" << min_epoch << " ref=" << mid_epoch << endl;
  }

  vector< pair<double,double> > data_x;
  vector< Estimate<double> > data_y;

  double xmin = table[0].freq[0];
  double xmax = xmin;

  for (unsigned irow = 0; irow < table.size(); irow++)
  {
    if (convert_epochs)
      table[irow].x0 = (table[irow].epoch - mid_epoch).in_days();

    double x0 = table[irow].x0;

    unsigned nchan = table[irow].freq.size();

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      data_x.push_back ( pair<double,double>( x0, table[irow].freq[ichan] ) );
      data_y.push_back ( table[irow].data[ichan] );

      xmin = min(xmin, table[irow].freq[ichan]);
      xmax = max(xmax, table[irow].freq[ichan]);
    }
  }

  cerr << "smint::fit_pspline fitting " << data_x.size() << " data points" << endl;
  spline.set_data (data_x, data_y);

  cerr << "xmin=" << xmin << " xmax=" << xmax << endl;

  if (spline_filename != "")
  {
    unsigned npts = 200;

    double x1del = (xmax-xmin)/(npts-1);
    double x0del = x0_span/(npts-1);
    double x0min = -0.5*x0_span;

    ofstream out (spline_filename.c_str());
    for (unsigned i=0; i<npts; i++)
    {
      double x0 = x0min + x0del * double(i);

      for (unsigned j=0; j<npts; j++)
      {
        double x1 = xmin + x1del * double(j);
  
        double y = spline.evaluate ( pair<double,double>(x0,x1) );
  
        out << x0 << " " << x1 << " " << y << endl;
      }
  
      out << endl;
    }
  }

#ifdef HAVE_PGPLOT

  if (plot_filename != "")
    cpgopen (plot_filename.c_str());

  for (unsigned irow = 0; irow < table.size(); irow++)
  {
    unsigned npts = 500;

    cpgpage();
    plot_data (table[irow].freq, table[irow].data);

    cpgsci(2); 
    double x0 = table[irow].x0;
    plot_model (spline, x0, npts, xmin, xmax);
  }

  if (plot_filename != "")
    cpgend ();

#endif

}

#endif // HAVE_SPLINTER

void smint::fit_polynomial (const vector< double >& data_x, const vector< Estimate<double> >& data_y)
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

  unsigned npts = 500;

  double xmin = *min_element(data_x.begin(), data_x.end());
  double xmax = *max_element(data_x.begin(), data_x.end());

  cerr << "xmin=" << xmin << " xmax=" << xmax << endl;

#ifdef HAVE_PGPLOT

  cpgpage();
  plot_data (data_x, data_y);

  cpgsci(2);
  plot_model (argument, scalar, npts, xmin, xmax);

#endif

  MEAL::LevenbergMarquardt<double> fit;

  float chisq = fit.init (axis_x, data_y, *scalar);
  cerr << "initial chisq = " << chisq << endl;

  unsigned iter = 1;
  unsigned not_improving = 0;

  while (not_improving < 25) {
    cerr << "iteration " << iter << endl;
    float nchisq = fit.iter (axis_x, data_y, *scalar);
    cerr << "     chisq = " << nchisq << endl;

    if (nchisq < chisq) {
      float diff_chisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diff_chisq/chisq < threshold && diff_chisq > 0) {
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
  plot_model (argument, scalar, npts, xmin, xmax);
#endif

  string model_text;
  scalar->print (model_text);
  cout << "ANSWER:\n" << model_text << endl;
}

#ifdef HAVE_PGPLOT

void smint::plot_data (const vector< double >& data_x,
                       const vector< Estimate<double> >& data_y)
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
}

void smint::plot_model (MEAL::Axis<double>& argument,
                        MEAL::Scalar* scalar,
                        unsigned npts, double xmin, double xmax)
{
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

#if HAVE_SPLINTER

#if 0

  // this should be wrapped in a template
  unsigned npts = 500;

  double xmin = *min_element(data_x.begin(), data_x.end());
  double xmax = *max_element(data_x.begin(), data_x.end());

  cerr << "xmin=" << xmin << " xmax=" << xmax << endl;

  cpgpage();
  plot_data (data_x, data_y);

  cpgsci(2);
  cpgslw(3);
  plot_model (spline, npts, xmin, xmax);

#endif

void smint::plot_model (SplineSmooth1D& spline,
                        unsigned npts, double xmin, double xmax)
{
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


/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  smint program;
  return program.main (argc, argv);
}

