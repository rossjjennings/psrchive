/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCovariancePlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/StokesCovariance.h"
#include "Pulsar/FourthMomentStats.h"

using namespace std;

Pulsar::StokesCovariancePlot::StokesCovariancePlot ()
{
}

TextInterface::Parser* Pulsar::StokesCovariancePlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesCovariancePlot::get_profiles (const Archive* data)
{

  if (verbose)
    cerr << "Pulsar::StokesCovariancePlot::get_profiles" << endl;

  Reference::To<const PolnProfile> profile = get_Stokes (data, isubint, ichan);
  Reference::To<const StokesCovariance> covar = profile->get_covariance();

  unsigned npol = 4;

  plotter.profiles.resize( npol );

  plotter.profiles[0] = covar->get_Profile(0);
  plotter.profiles[1] = covar->get_Profile(4);
  plotter.profiles[2] = covar->get_Profile(7);
  plotter.profiles[3] = covar->get_Profile(9);

  Reference::To<FourthMomentStats> stats = new FourthMomentStats(profile);
  stats->eigen();
  plotter.profiles[0] = stats->get_covariance()->get_Profile(0);
  for (unsigned i=0; i<3; i++)
    plotter.profiles[i+1] = stats->get_eigen_value(i);
}

Pulsar::StokesCovariancePlot::Interface::Interface (StokesCovariancePlot* obj)
{
  if (obj)
    set_instance (obj);

  import ( FluxPlot::Interface() );
}
