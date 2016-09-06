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
  bin = -1;
  max_bin = false;
}

TextInterface::Parser* Pulsar::StokesCrossCovariancePlot::get_interface ()
{
  return new Interface (this);
}

unsigned get_icross (unsigned nbin, unsigned ibin, unsigned jbin)
{
  if (jbin < ibin)
    std::swap (ibin, jbin);
      
  // icross = nbin + nbin-1 + nbin-2 + nbin-3,
  //          where the number of terms = ibin
  // then offset by jbin, which starts at ibin

  return ibin * nbin - (ibin * (ibin-1) / 2) + (jbin - ibin);
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
  
  nbin = matrix->get_nbin();
  unsigned npol = matrix->get_npol();

  if (npol != 4)
    throw Error (InvalidState,
		 "Pulsar::StokesCrossCovariancePlot::get_profiles",
		 "CovarianceMatrix::npol=%u != 4", npol);

  const vector<double>& data = matrix->get_data();
  unsigned idat=0;

  stokes_crossed.resize( nbin * (nbin+1) / 2 );
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

  if (max_bin)
    bin = archive->get_Profile(0,0,0)->find_max_bin();

  if (bin >= 0)
    plot_bin ();
  else
    plot_lags ();
}

void Pulsar::StokesCrossCovariancePlot::plot_bin ()
{
  unsigned ndim = 10;
  plotter.profiles.resize( ndim );

  unsigned jbin = bin;
  assert (jbin < nbin);

  unsigned idim=0;
  unsigned npol=4;

  for (unsigned ipol=0; ipol < npol; ipol++)
    for (unsigned jpol=ipol; jpol < npol; jpol++)
    {
      Profile* profile = new Profile(nbin);
      plotter.profiles[idim] = profile;
      idim ++;
    
      float* amps = profile->get_amps();

      for (unsigned ibin=0; ibin < nbin; ibin++)
      {
	unsigned icross = get_icross (nbin, ibin, jbin);
      
	if (icross >= stokes_crossed.size())
	  throw Error (InvalidRange,
		       "Pulsar::StokesCrossCovariancePlot::plot_bin",
		       "nbin=%u ibin=%u jbin=%u -> icross=%u >= ncross=%u",
		       nbin, ibin, jbin, icross, stokes_crossed.size());
 
	Matrix<4,4,double>& c = stokes_crossed[icross];

	amps[ibin] = c[ipol][jpol];
      }
    }

  assert (idim == ndim);
}

void Pulsar::StokesCrossCovariancePlot::plot_lags ()
{
  plotter.profiles.resize( lags+1 );

  for (unsigned ilag=0; ilag < lags+1; ilag++)
  {
    Profile* profile = new Profile(nbin);
    plotter.profiles[ilag] = profile;
    float* amps = profile->get_amps();

    for (unsigned ibin=0; ibin < nbin; ibin++)
    {
      unsigned jbin = (ibin + ilag) % nbin;

      unsigned icross = get_icross (nbin, ibin, jbin);
      
      if (icross >= stokes_crossed.size())
	throw Error (InvalidRange,
		     "Pulsar::StokesCrossCovariancePlot::plot_lags",
		     "nbin=%u ibin=%u jbin=%u -> icross=%u >= ncross=%u",
		     nbin, ibin, jbin, icross, stokes_crossed.size());
 
      Matrix<4,4,double>& c = stokes_crossed[icross];

      amps[(ibin+ilag/2)%nbin] = c[covar.first][covar.second];
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
       "lags", "number of lags >0 to plot");

  add( &StokesCrossCovariancePlot::get_bin,
       &StokesCrossCovariancePlot::set_bin,
       "bin", "phase bin to plot (if >0)");

  add( &StokesCrossCovariancePlot::get_max_bin,
       &StokesCrossCovariancePlot::set_max_bin,
       "max", "find phase bin to plot");
}


