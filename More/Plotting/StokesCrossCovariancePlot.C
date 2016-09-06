/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCrossCovariancePlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/CovarianceMatrix.h"

#include "pairutil.h"
#include "Matrix.h"

#include <assert.h>

using namespace std;

Pulsar::StokesCrossCovariancePlot::StokesCrossCovariancePlot ()
{
  covar.first = covar.second = 0;
  lags = 4;
}

TextInterface::Parser* Pulsar::StokesCrossCovariancePlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesCrossCovariancePlot::get_profiles (const Archive* archive)
{

  if (verbose)
    cerr << "Pulsar::StokesCrossCovariancePlot::get_profiles" << endl;

  const CovarianceMatrix* matrix = archive->get<CovarianceMatrix>();

  if (!matrix)
    throw Error (InvalidState,
		 "Pulsar::StokesCrossCovariancePlot::get_profiles",
		 "archive does not contain a CovarianceMatrix extension");
  
  unsigned nbin = matrix->get_nbin();
  unsigned npol = matrix->get_npol();

  if (npol != 4)
    throw Error (InvalidState,
		 "Pulsar::StokesCrossCovariancePlot::get_profiles",
		 "CovarianceMatrix::npol=%u != 4", npol);

  const vector<double>& data = matrix->get_data();
  unsigned idat=0;

  vector< Matrix<4,4,double> > stokes_crossed ( nbin * (nbin+1) / 2 );
  unsigned icov=0;
  
  for (unsigned ibin = 0; ibin < nbin ; ibin ++)
  {
    for (unsigned jbin = ibin; jbin < nbin ; jbin ++)
    {
      Matrix<4,4,double>& c = stokes_crossed[icov];
      icov ++;
      
      for (unsigned ipol=0; ipol < npol; ipol++)
	for (unsigned jpol = (ibin == jbin) ? ipol : 0; jpol < npol ; jpol++)
	{
	  c[ipol][jpol] = data.at(idat);
	  idat ++;

	  if (ibin == jbin)
	    c[jpol][ipol] = c[ipol][jpol];
	}
    }
  }

  assert (icov == stokes_crossed.size());
  assert (idat == data.size());

  plotter.profiles.resize( lags+1 );

  for (unsigned ilag=0; ilag < lags+1; ilag++)
  {
    Profile* profile = new Profile(nbin);
    plotter.profiles[ilag] = profile;
    float* amps = profile->get_amps();

    for (unsigned ibin=0; ibin < nbin; ibin++)
    {
      unsigned jbin = (ibin + ilag) % nbin;

      // icross = nbin + nbin-1 + nbin-2 + nbin-3,
      //          where the number of terms = ibin
      // then offset by jbin, which starts at ibin

      unsigned icross = ibin * nbin - (ibin * (ibin-1) / 2) + (jbin - ibin);

      if (icross >= stokes_crossed.size())
	throw Error (InvalidRange,
		     "Pulsar::StokesCrossCovariancePlot::get_profiles",
		     "nbin=%u ibin=%u jbin=%u -> icross=%u >= ncross=%u",
		     nbin, ibin, jbin, icross, stokes_crossed.size());
 
      Matrix<4,4,double>& c = stokes_crossed[icross];

      amps[ibin] = c[covar.first][covar.second];
    }
  }
}





Pulsar::StokesCrossCovariancePlot::Interface::Interface (StokesCrossCovariancePlot* obj)
{
  if (obj)
    set_instance (obj);

  import ( FluxPlot::Interface() );

  add( &StokesCrossCovariancePlot::get_covar,
       &StokesCrossCovariancePlot::set_covar,
       "covar", "indeces of covariance matrix to plot");

  add( &StokesCrossCovariancePlot::get_lags,
       &StokesCrossCovariancePlot::set_lags,
       "lags", "range of lags to plot");
}


