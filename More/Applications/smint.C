/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
   smint - Smooth and interpolate something, where something might be
         - PolnCalibratorExtension model parameters
         - CalibratorStokes parameters
         - Pulsar spectrum (e.g. in each phase bin)
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/Profile.h"

#ifdef HAVE_PGPLOT
#include <cpgplot.h>
#include "EstimatePlotter.h"
#include "Pulsar/PlotOptions.h"
#endif

#ifdef HAVE_SPLINTER
#include "Pulsar/SplineSmooth.h"
#endif

#include "MEAL/LevenbergMarquardt.h"
#include "MEAL/Gaussian.h"
#include "MEAL/Polynomial.h"
#include "MEAL/Axis.h"

using namespace Pulsar;
using namespace std;

//
//! Smooth and interpolate, mostly as a function of radio frequency
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

  void fit_polynomial (const vector< double >& data_x,
                       const vector< Estimate<double> >& data_y);

  void plot_data (const vector< double >& data_x,
                  const vector< Estimate<double> >& data_y);

  void plot_model (MEAL::Axis<double>& argument,
                   MEAL::Scalar* scalar,
                   unsigned npts, double xmin, double xmax);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  double pspline_alpha;

  unsigned freq_order;
  unsigned time_order;
  float threshold;

  class row
  {
  public:
    MJD epoch;
    vector<double> freq;
    vector< Estimate<double> > data;
  };

  std::vector<row> table;

#if HAVE_SPLINTER

  void fit_pspline (const vector< double >& data_x,
                    const vector< Estimate<double> >& data_y);

  void fit_pspline (const vector<row>& table);

  void plot_model (SplineSmooth1D& spline,
                   unsigned npts, double xmin, double xmax);

  void plot_model (SplineSmooth2D& spline, double x0,
                   unsigned npts, double xmin, double xmax);

#endif

};


/*!


*/

smint::smint ()
  : Application ("smint", "smooth and interpolate")
{
  add( new Pulsar::StandardOptions );

#ifdef HAVE_PGPLOT
  add( new Pulsar::PlotOptions );
#endif

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

void smint::process (Pulsar::Archive* archive)
{
  Reference::To< PolnCalibratorExtension > ext;
  ext = archive->get<PolnCalibratorExtension>();
  if (!ext)
    throw Error (InvalidState, "smint::process",
                 "Archive does not contain PolnCalibratorExtension");

  table.resize( table.size() + 1 );

  std::vector< double >& data_x = table.back().freq;
  std::vector< Estimate<double> >& data_y = table.back().data;

  table.back().epoch = ext->get_epoch();

  unsigned nchan = ext->get_nchan();
  unsigned iparam = 3;

  double centre_frequency = archive->get_centre_frequency ();

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!ext->get_valid(ichan))
      continue;

    double freq = ext->get_centre_frequency (ichan);

    Estimate<double> val = ext->get_Estimate (iparam, ichan);

    data_x.push_back( freq - centre_frequency );
    data_y.push_back ( val );
  }
}

void smint::finalize ()
{
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
      throw Error (InvalidState, "smint::finalize",
                   "2-D smoothing with polynomials not implemented");

#if HAVE_SPLINTER
    fit_pspline (table);
#else
    throw Error (InvalidState, "smint::finalize",
                 "2-D smoothing requires SPLINTER library");
#endif
  } 
}

#if HAVE_SPLINTER

void smint::fit_pspline (const vector< double >& data_x, const vector< Estimate<double> >& data_y)
{
  SplineSmooth1D spline;
  spline.set_alpha (pspline_alpha);
  spline.set_data (data_x, data_y);

  unsigned npts = 500;

  double xmin = *min_element(data_x.begin(), data_x.end());
  double xmax = *max_element(data_x.begin(), data_x.end());

  cerr << "xmin=" << xmin << " xmax=" << xmax << endl;

#ifdef HAVE_PGPLOT

  cpgpage();
  plot_data (data_x, data_y);

  cpgsci(2);
  plot_model (spline, npts, xmin, xmax);

#endif
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

  double diff = (max_epoch - min_epoch).in_days();

  cerr << "smint::fit_pspline data span " << diff << " days" << endl;

  MJD mid_epoch = min_epoch + MJD(0.5 * diff);

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

#ifdef HAVE_PGPLOT
  for (unsigned irow = 0; irow < table.size(); irow++)
  {
    unsigned npts = 500;

    cpgpage();
    plot_data (table[irow].freq, table[irow].data);

    cpgsci(2); 
    double x0 = (table[irow].epoch - mid_epoch).in_days();
    plot_model (spline, x0, npts, xmin, xmax);
  }
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
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diffchisq/chisq < threshold && diffchisq > 0) {
        cerr << "no big diff in chisq = " << diffchisq << endl;
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

