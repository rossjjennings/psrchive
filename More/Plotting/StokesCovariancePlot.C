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

void Pulsar::StokesCovariancePlot::get_profiles (const Archive* data)
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
    get_frame()->get_y_axis()->set_label("\\(2255)C\\dii");

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
    get_frame()->get_y_axis()->set_label("C\\d0i");

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

    stats->set_duration (subint->get_duration());
    stats->set_folding_period (subint->get_folding_period());

    if (ichan.get_integrate())
      stats->set_bandwidth (subint->effective_bandwidth());
    else
      stats->set_bandwidth (subint->get_bandwidth() / subint->get_nchan());

    plotter.profiles[0] = stats->get_modulation_index();
  }


}





Pulsar::StokesCovariancePlot::Interface::Interface (StokesCovariancePlot* obj)
{
  if (obj)
    set_instance (obj);

  import ( FluxPlot::Interface() );

  add( &StokesCovariancePlot::get_what,
       &StokesCovariancePlot::set_what,
       "what", "What to plot (var, eigen, covar, reg, beta)");
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
  case StokesCovariancePlot::RegressionCoefficients:
    return os << "reg";
  case StokesCovariancePlot::ModulationIndex:
    return os << "beta";
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
  else if (unit == "reg")
    what = StokesCovariancePlot::RegressionCoefficients;
  else if (unit == "beta")
    what = StokesCovariancePlot::ModulationIndex;
  else
    is.setstate(std::istream::failbit);

  return is;
}
