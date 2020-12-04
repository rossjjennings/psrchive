/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCovariancePlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/StokesCovariance.h"
#include "Pulsar/FourthMomentStats.h"

using namespace std;

Pulsar::StokesCovariancePlot::StokesCovariancePlot ()
{
  what = EigenValues;
}

TextInterface::Parser* Pulsar::StokesCovariancePlot::get_interface ()
{
  return new Interface (this);
}

void set (vector<int>& to, const std::string& from)
{
  if (from == "")
    return;

  to.resize( from.length() );
  for (unsigned i=0; i<from.length(); i++)
    to[i] = from[i] - '0';
}

void Pulsar::StokesCovariancePlot::get_profiles (const Archive* data) try
{

  if (verbose)
    cerr << "Pulsar::StokesCovariancePlot::get_profiles" << endl;

  Reference::To<const Integration> subint = get_Integration (data, isubint);
  Reference::To<const PolnProfile> profile = get_Stokes (subint, ichan);
  Reference::To<const StokesCovariance> covar = profile->get_covariance();

  Reference::To<FourthMomentStats> stats = new FourthMomentStats(profile);

  if (what == Variances)
  {
    unsigned npol = 4;

    plotter.profiles.resize( npol );

    plotter.profiles[0] = covar->get_Profile(0);
    plotter.profiles[1] = covar->get_Profile(4);
    plotter.profiles[2] = covar->get_Profile(7);
    plotter.profiles[3] = covar->get_Profile(9);
  }

  else if (what == EigenValues)
  {
    get_frame()->get_y_axis()->set_label("\\(2255)C\\dii", false);

    unsigned npol = 4;

    plotter.profiles.resize( npol );
    plotter.plot_sls.resize( npol );
    plotter.plot_slw.resize( npol );
    
    stats->eigen();
    plotter.profiles[0] = stats->get_covariance()->get_Profile(0);
    for (unsigned i=0; i<3; i++)
      plotter.profiles[i+1] = stats->get_eigen_value(i);

    for (unsigned i=0; i<4; i++)
    {
      Profile* tmp = plotter.profiles[i]->clone();
      tmp->square_root();
      plotter.profiles[i] = tmp;
      plotter.plot_sls[i] = 1;
      plotter.plot_slw[i] = 3;
    }
  }

  else if (what == NaturalCovariances)
  {
    get_frame()->get_y_axis()->set_label("C\\d0i", false);

    plotter.plot_sci.resize(3);
    plotter.plot_sls.resize(3);
    plotter.plot_slw.resize(3);

    plotter.profiles.resize( 3 );
    
    stats->eigen();
    for (unsigned i=0; i<3; i++)
    {
      plotter.profiles[i] = stats->get_natural_covariance(i);
      plotter.plot_sci[i] = i+2;
      plotter.plot_sls[i] = 1;
      plotter.plot_slw[i] = 3;
    }
  }

  else if (what == NonOrthogonality)
  {
    get_frame()->get_y_axis()->set_label("cos/sin(\\gh)", false);

    plotter.profiles.resize(3);
    plotter.plot_sci.resize(3);
    plotter.plot_sls.resize(3);
    plotter.plot_slw.resize(3);

    stats->eigen();

    for (unsigned i=0; i<3; i++)
    {
      plotter.plot_sci[i] = i+1;
      plotter.plot_sls[i] = 1;
      plotter.plot_slw[i] = 3;
    }

    plotter.profiles[0] = stats->get_norm_theta();
    plotter.profiles[1] = stats->get_cos_theta();
    plotter.profiles[2] = stats->get_sin_theta();
  }

  else if (what == Theta)
  {
    get_frame()->get_y_axis()->set_label("\\gh (rad)", false);

    plotter.profiles.resize( 1 );
    stats->eigen();
    plotter.profiles[0] = stats->get_nonorthogonality();
  }


  else if (what == RegressionCoefficients)
  {
    plotter.profiles.resize( 3 );
    
    stats->eigen();
    for (unsigned i=0; i<3; i++)
      plotter.profiles[i] = stats->get_regression_coefficient(i);
  }

  else if (what == ModulationIndex)
  {
    plotter.profiles.resize( 1 );
    plotter.profiles[0] = stats->get_modulation_index();

    double num=0, den=0;
    unsigned nbin = profile->get_nbin();
    const float* I = profile->get_Profile(0)->get_amps();
    const float* beta = plotter.profiles[0]->get_amps();

    for (unsigned ibin=0; ibin < nbin; ibin++)
      if (beta[ibin] != 0.0)
      {
        double x = beta[ibin] * I[ibin];
        num += x * x;
        den += I[ibin] * I[ibin];
      }

    cerr << "bar beta = " << sqrt(num/den) << endl;

  }

  else if (what == DegreeOfPolarization)
  {
    get_frame()->get_y_axis()->set_label("Degree of Polarization", false);

    Reference::To<Profile> dop_mean = profile->get_Profile(0)->clone();
    Reference::To<Profile> dop_eigen = dop_mean->clone();
    
    plotter.profiles.resize( 2 );
    plotter.plot_sls.resize( 2 );
    plotter.plot_slw.resize( 2 );
    plotter.plot_sci.resize( 2 );

    plotter.profiles[0] = dop_mean;
    plotter.plot_sls[0] = 1;
    plotter.plot_slw[0] = 3;
    plotter.plot_sci[0] = 1;

    plotter.profiles[1] = dop_eigen;
    plotter.plot_sls[1] = 2;
    plotter.plot_slw[1] = 3;
    plotter.plot_sci[1] = 1;

    Reference::To<Profile> beta = stats->get_modulation_index();

    stats->eigen();

    const float* e0 = stats->get_eigen_value(0)->get_amps();
    const float* e1 = stats->get_eigen_value(1)->get_amps();
    const float* e2 = stats->get_eigen_value(2)->get_amps();
    const float* mval = beta->get_amps();

    float* dpm = dop_mean->get_amps();
    float* dpe = dop_eigen->get_amps();
    
    unsigned nbin = dop_mean->get_nbin();

    PhaseWeight* baseline = stats->get_stats()->get_baseline ();

    // total intensity
    const Profile* intensity = profile->get_Profile(0);
    double offI_mean, offI_var;
    baseline->stats (intensity, &offI_mean, &offI_var);

    const Profile* minor = stats->get_eigen_value(1);
    double off_min_mean, off_min_var;
    baseline->stats (minor, &off_min_mean, &off_min_var);

    const Profile* major = stats->get_eigen_value(0);
    double off_maj_mean, off_maj_var;
    baseline->stats (major, &off_maj_mean, &off_maj_var);

    const float* I = intensity->get_amps();

    // 6-sigma ... TODO: should be adjustable
    float threshold = 6.0;
    
    for (unsigned i=0; i<nbin; i++)
    {
      float Ival = I[i] - offI_mean;
      double Isq = Ival * Ival;

      if (Isq < threshold * threshold * offI_var)
      {
	dpm[i] = dpe[i] = 0;
	continue;
      }

      float min_val = e1[i] - off_min_mean;
      double min_sq = min_val * min_val;
      
      if (min_sq < threshold * threshold * off_min_var)
      {
	dpm[i] = dpe[i] = 0;
	continue;
      }

      float maj_val = e0[i] - off_maj_mean;
      double maj_sq = maj_val * maj_val;
      
      if (maj_sq < threshold * threshold * off_maj_var)
      {
	dpm[i] = dpe[i] = 0;
	continue;
      }

      double eps_sq = maj_val / min_val;

      if (eps_sq < 1)
       {
	dpm[i] = dpe[i] = 0;
	continue;
      }

      double msq = mval[i] * mval[i];
      double psq = (1+msq) * (eps_sq - 1) / (1 + 2*msq + eps_sq * (1+msq));
      dpe[i] = sqrt( psq );

      Stokes<float> stokes = profile->get_Stokes(i);
      dpm[i] = stokes.abs_vect() / stokes.get_scalar();

    }
    
  }

  set (plotter.plot_sci, plot_colours);
  set (plotter.plot_sls, plot_lines);
  set (plotter.plot_slw, plot_widths);
}

 catch (Error& error)
   {
     throw error += "Pulsar::StokesCovariancePlot::get_profiles";
   }



Pulsar::StokesCovariancePlot::Interface::Interface (StokesCovariancePlot* obj)
{
  if (obj)
    set_instance (obj);

  import ( FluxPlot::Interface() );

  add( &StokesCovariancePlot::get_what,
       &StokesCovariancePlot::set_what,
       "what", "What to plot (var, eigen, covar, reg, beta)");

  add( &StokesCovariancePlot::get_plot_colours,
       &StokesCovariancePlot::set_plot_colours,
       "ci", "PGPLOT colour index for each value" );

  add( &StokesCovariancePlot::get_plot_lines,
       &StokesCovariancePlot::set_plot_lines,
       "ls", "PGPLOT line style for each value" );

  add( &StokesCovariancePlot::get_plot_widths,
       &StokesCovariancePlot::set_plot_widths,
       "lw", "PGPLOT line width for each value" );

}



std::ostream& Pulsar::operator << (std::ostream& os,
				   StokesCovariancePlot::What what)
{
  switch (what) {
  case StokesCovariancePlot::Variances:
    return os << "var";
  case StokesCovariancePlot::EigenValues:
    return os << "eigen";
  case StokesCovariancePlot::NaturalCovariances:
    return os << "covar";
  case StokesCovariancePlot::NonOrthogonality:
    return os << "nonortho";
  case StokesCovariancePlot::Theta:
    return os << "theta";
  case StokesCovariancePlot::RegressionCoefficients:
    return os << "reg";
  case StokesCovariancePlot::ModulationIndex:
    return os << "beta";
  case StokesCovariancePlot::DegreeOfPolarization:
    return os << "dop";
  default:
    return os << "unknown";
  }
}

std::istream& Pulsar::operator >> (std::istream& is,
				   StokesCovariancePlot::What& what)
{
  std::streampos pos = is.tellg();
  std::string unit;
  is >> unit;

  if (unit == "var")
    what = StokesCovariancePlot::Variances;
  else if (unit == "eigen")
    what = StokesCovariancePlot::EigenValues;
  else if (unit == "covar")
    what = StokesCovariancePlot::NaturalCovariances;
  else if (unit == "nonortho")
    what = StokesCovariancePlot::NonOrthogonality;
  else if (unit == "theta")
    what = StokesCovariancePlot::Theta;
  else if (unit == "reg")
    what = StokesCovariancePlot::RegressionCoefficients;
  else if (unit == "beta")
    what = StokesCovariancePlot::ModulationIndex;
  else if (unit == "dop")
    what = StokesCovariancePlot::DegreeOfPolarization;
  else
    is.setstate(std::istream::failbit);

  return is;
}
