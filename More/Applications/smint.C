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
#include <data_table.h>
#include <bspline.h>
#include <bspline_builders.h>
using namespace SPLINTER;
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

  //! Process the given archive
  void process (Pulsar::Archive*);

  void fit_polynomial (const vector< double >& data_x,
                       const vector< Estimate<double> >& data_y);

#if HAVE_SPLINTER
  void fit_pspline (const vector< double >& data_x, 
                           const vector< Estimate<double> >& data_y);

  void plot_model (BSpline& spline,
                   unsigned npts, double xmin, double xmax);

#endif

  void plot_model (MEAL::Axis<double>& argument,
                   MEAL::Scalar* scalar,
                   unsigned npts, double xmin, double xmax);

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  double pspline_alpha;

  unsigned freq_order;
  unsigned time_order;
  string display;
  float threshold;

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
  display = "?";
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

  std::vector< double > data_x;             // x-ordinate of data
  std::vector< Estimate<double> > data_y;   // y-ordinate of data with error

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

  if (pspline_alpha)
    fit_pspline (data_x, data_y);
  else
    fit_polynomial (data_x, data_y);
}

void smint::fit_pspline (const vector< double >& data_x, const vector< Estimate<double> >& data_y)
{
  // Create new DataTable to manage samples
  DataTable samples;

  // when switching to two-dimensional fit
  // std::vector<double> x(2);
  double x;
  double y;

  vector<double> weights;

  for (unsigned i=0; i<data_x.size(); i++)
  {
    samples.add_sample (data_x[i], data_y[i].val);
    weights.push_back (1.0/data_y[i].var);
  }

/**
 * Create a B-spline that smooths the sample points using regularization (weight decay).
 * @param data A table of sample points on a regular grid.
 * @param degree The degree of the B-spline basis functions. Default degree is 3 (cubic).
 * @param smoothing Type of regularization to use - see BSpline::Smoothing. Default is smoothing is BSpline::PSLINE.
 * @param alpha Smoothing/regularization factor.
 * @param weights Sample weights.
 * @return A B-spline that smooths the sample points.

BSpline bspline_smoother(const DataTable &data, unsigned int degree = 3,
                         BSpline::Smoothing smoothing = BSpline::Smoothing::PSPLINE, double alpha = 0.1,
                         std::vector<double> weights = std::vector<double>());
*/

  // Build penalized B-spline (P-spline) that smooths the samples
  unsigned int degree = 3;
  BSpline::Smoothing smoothing = BSpline::Smoothing::PSPLINE;

  BSpline pspline = bspline_smoother(samples, degree, smoothing, pspline_alpha, weights);

  unsigned npts = 500;

  double xmin = *min_element(data_x.begin(), data_x.end());
  double xmax = *max_element(data_x.begin(), data_x.end());

  cerr << "xmin=" << xmin << " xmax=" << xmax << endl;

#ifdef HAVE_PGPLOT

  //
  // Plot the data
  //

  EstimatePlotter plot;
  plot.add_plot (data_x, data_y);

  cpgsch (1.5);
  cpgsci (1);

  cpgpage();
  plot.plot();

  cpgbox ("bcinst", 0,0, "bcinst", 0,0);


  //
  // Plot the input model
  //

  cpgsci(2);
  plot_model (pspline, npts, xmin, xmax);

#endif

}

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

  //
  // Plot the data
  //

  EstimatePlotter plot;
  plot.add_plot (data_x, data_y);

  cpgsch (1.5);
  cpgsci (1);

  cpgpage();
  plot.plot();

  cpgbox ("bcinst", 0,0, "bcinst", 0,0);


  //
  // Plot the input model
  //

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

void smint::plot_model (BSpline& spline,
                 unsigned npts, double xmin, double xmax)
{
  double xdel = (xmax-xmin)/(npts-1);
  vector<double> xval (1);
  for (unsigned i=0; i<npts; i++)
  {
    xval[0] = xmin + xdel * double(i);
   
    vector<double> y = spline.eval (xval);

    if (i==0)
      cpgmove (xval[0],y[0]);
    else
      cpgdraw (xval[0],y[0]);
  }

}

#endif  // HAVE_SPLINTER1
#endif  // HAVE_PGPLOT



/*!

  The standard C/C++ main function simply calls Application::main

*/

int main (int argc, char** argv)
{
  smint program;
  return program.main (argc, argv);
}

