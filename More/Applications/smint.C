/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
   smint - Smooth and interpolate data, where data might be
         - MEM/METM model parametersm, including PolnCalibratorExtension
           and CalibratorStokes parameters
         - Pulsar spectrum (e.g. in each phase bin)
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CalibratorStokes.h"
#include "Pulsar/Profile.h"

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

  // a spectrum of estimates observed at a given epoch
  class row
  {
  public:
    MJD epoch;
    vector< double > freq;
    vector< Estimate<double> > data;
  };

  // a set of spectra from an indexed slice of data
  class set
  {
  public:
    unsigned index;
    std::vector<row> table;
  };

  // pulsar profile [ipol][ibin] to be smoothed
  vector< vector<set> > profile_data;

  // PolnCalibratorExtension [iparam] to be smoothed
  vector<set> pcal_data;

  // CalibratorStokes [ipol] to be smoothed
  vector<set> cal_stokes_data;

  // the centre frequency of the data
  double centre_frequency;
  vector<double> channel_frequency;
  string filename;

  // load data from a container into a set::table
  template<class Container>
  void load (vector<set>& data, Container* ext, const MJD& epoch);

  // unload text to file
  void unload (const string& filename, const vector<row>& table);

  string spline_filename;
  string plot_filename;

  void fit (vector<row>& table);

#if HAVE_SPLINTER

  void fit_pspline (const vector< double >& data_x,
                    const vector< Estimate<double> >& data_y);

  void fit_pspline (const vector<row>& table);

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

#if HAVE_SPLINTER
  // add a blank line and a header to the output of -h
  menu.add ("\n" "Penalized spline (p-spline) fitting options:");

  arg = menu.add (pspline_alpha, 'p', "double");
  arg->set_help ("p-spline smoothing factor 'alpha'");
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

      data_x.push_back( channel_frequency[ichan] );
      data_y.push_back( ext->get_Estimate (iparam, ichan) );
    }
  }
}

void smint::process (Pulsar::Archive* archive)
{
  Reference::To< PolnCalibratorExtension > ext;
  ext = archive->get<PolnCalibratorExtension>();
  if (ext)
  {
    if (pcal_data.size() == 0)
    {
      // smooth the receptor parameters
      pcal_data.resize (4);

      for (unsigned i=0; i<pcal_data.size(); i++)
        // +3 to ignore the backend (gain and phase)
        // and smooth only the receptor parameters
        pcal_data[i].index = i + 3;

      centre_frequency = archive->get_centre_frequency();
      filename = archive->get_filename();

      channel_frequency.resize( ext->get_nchan() );
      for (unsigned i=0; i<ext->get_nchan(); i++)
        channel_frequency[i] = ext->get_centre_frequency(i) - centre_frequency;
    }
    else
    {
      if (archive->get_centre_frequency() != centre_frequency)
        throw Error (InvalidState, "smint::process", "centre frequency mismatch\n"
                     "\t" + filename + " freq=" + tostring(centre_frequency) + "\n"
                     "\t" + archive->get_filename() + " freq=" + tostring(archive->get_centre_frequency()) );

      if (ext->get_nchan() != channel_frequency.size())
        throw Error (InvalidState, "smint::process", "nchan mismatch\n"
                     "\t" + filename + " freq=" + tostring(channel_frequency.size()) + "\n"
                     "\t" + archive->get_filename() + " freq=" + tostring(ext->get_nchan()) );

      for (unsigned i=0; i<ext->get_nchan(); i++)
        if (channel_frequency[i] != ext->get_centre_frequency(i) - centre_frequency)
          throw Error (InvalidState, "smint::process", "channel frequency mismatch\n"
                     "\t" + filename + " freq=" + tostring(centre_frequency+channel_frequency[i]) + "\n"
                     "\t" + archive->get_filename() + " freq=" + tostring(ext->get_centre_frequency(i)) );
    }

    MJD epoch = ext->get_epoch();

    load (pcal_data, ext.get(), epoch);

    Reference::To< CalibratorStokes > cal;
    cal = archive->get<CalibratorStokes>();
    if (cal)
    {
      if (cal_stokes_data.size() == 0)
      {
        // smooth Q, U, and V
        cal_stokes_data.resize (3);
  
        for (unsigned i=0; i<pcal_data.size(); i++)
          cal_stokes_data[i].index = i;
      }
    
      load (cal_stokes_data, cal.get(), epoch);
    }
  }
}

void smint::fit (vector<row>& table)
{
  if (table.size() == 0)
    throw Error (InvalidState, "smint::fit", "empty table");

  if (table.size() == 1)
  {
#if HAVE_SPLINTER
    if (pspline_alpha)
      fit_pspline (table.front().freq, table.front().data);
    else
#endif
      fit_polynomial (table.front().freq, table.front().data);
  }
  else if (table.size() > 1)
  {
    if (!pspline_alpha)
      throw Error (InvalidState, "smint::fit",
                   "2-D smoothing with polynomials not implemented");

#if HAVE_SPLINTER
    fit_pspline (table);
#else
    throw Error (InvalidState, "smint::fit",
                 "2-D smoothing requires SPLINTER library");
#endif
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

  for (unsigned i=0; i < pcal_data.size(); i++)
  {
    cerr << "smint: fitting PolnCalibrator "
            "iparam=" << pcal_data[i].index << endl;

    string idx = tostring(pcal_data[i].index);
    spline_filename = "pcal_spline_" + idx + ".txt";
    plot_filename = "pcal_fit_" + idx + ".eps/cps";

    fit (pcal_data[i].table);
  }

  for (unsigned i=0; i < cal_stokes_data.size(); i++)
  {
    cerr << "smint: fitting CalibratorStokes iparam=" << i << endl;

    string idx = tostring(cal_stokes_data[i].index);
    spline_filename = "cal_stokes_spline_" + idx + ".txt";
    plot_filename = "cal_stokes_fit_" + idx + ".eps/cps";

    fit (cal_stokes_data[i].table);
  }
}

#if HAVE_SPLINTER

void smint::fit_pspline (const vector< double >& data_x,
                         const vector< Estimate<double> >& data_y)
{
  SplineSmooth1D spline;
  spline.set_alpha (pspline_alpha);
  spline.set_data (data_x, data_y);
}


void smint::fit_pspline (const vector<row>& table)
{ 
  SplineSmooth2D spline;
  spline.set_alpha (pspline_alpha);

  MJD min_epoch = table.front().epoch;
  MJD max_epoch = table.front().epoch;

  for (unsigned irow = 1; irow < table.size(); irow++)
  {
    if (table[irow].epoch > max_epoch)
      max_epoch = table[irow].epoch;
    if (table[irow].epoch < min_epoch)
      min_epoch = table[irow].epoch;
  }

  double span_days = (max_epoch - min_epoch).in_days();

  cerr << "smint::fit_pspline data span " << span_days << " days" << endl;

  MJD mid_epoch = min_epoch + MJD(0.5 * span_days);

  cerr << "smint::fit_pspline min=" << min_epoch << " ref=" << mid_epoch << endl;

  vector< pair<double,double> > data_x;
  vector< Estimate<double> > data_y;

  double xmin = table[0].freq[0];
  double xmax = xmin;

  for (unsigned irow = 0; irow < table.size(); irow++)
  {
    double x0 = (table[irow].epoch - mid_epoch).in_days();

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
    double x0del = span_days/(npts-1);
    double x0min = -0.5*span_days;

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

  if (plot_filename == "")
    return;

  cpgopen (plot_filename.c_str());

  for (unsigned irow = 0; irow < table.size(); irow++)
  {
    unsigned npts = 500;

    cpgpage();
    plot_data (table[irow].freq, table[irow].data);

    cpgsci(2); 
    double x0 = (table[irow].epoch - mid_epoch).in_days();
    plot_model (spline, x0, npts, xmin, xmax);
  }

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

