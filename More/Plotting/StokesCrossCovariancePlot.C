/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StokesCrossCovariancePlot.h"
#include "Pulsar/StokesCrossCovariance.h"

#include "Pulsar/Archive.h"
#include "Pulsar/CrossCovarianceMatrix.h"
#include "Pulsar/ProfileStats.h"

#include "pairutil.h"
#include "Matrix.h"

#include <fstream>
#include <iomanip>

#include <assert.h>

using namespace std;

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

Pulsar::StokesCrossCovariancePlot::StokesCrossCovariancePlot ()
{
  covar.first = covar.second = 0;
  lags = 4;
  bin = -1;
  max_bin = false;
  splot_output = false;

  stats = new ProfileStats;
  parser = stats->get_interface ();
}

TextInterface::Parser* Pulsar::StokesCrossCovariancePlot::get_interface ()
{
  return new Interface (this);
}

void Pulsar::StokesCrossCovariancePlot::set_config (const std::string& str)
{ 
  cerr << parser->process(str);
}

void Pulsar::StokesCrossCovariancePlot::get_profiles (const Archive* archive)
{
  if (verbose)
    cerr << "Pulsar::StokesCrossCovariancePlot::get_profiles" << endl;

  const CrossCovarianceMatrix* matrix = archive->get<CrossCovarianceMatrix>();
  if (!matrix)
    throw Error (InvalidState,
		 "Pulsar::StokesCrossCovariancePlot::get_profiles",
		 "archive does not contain a CrossCovarianceMatrix extension");

  stokes_crossed = new StokesCrossCovariance (matrix);
  
  if (max_bin)
    bin = archive->get_Profile(0,0,0)->find_max_bin();

  if (!expression.empty())
    plot_stats();
  else if (bin >= 0)
    plot_bin ();
  else
    plot_lags ();
}

void Pulsar::StokesCrossCovariancePlot::plot_bin ()
{
  unsigned ndim = 4;
  plotter.profiles.resize( ndim );
  plotter.plot_sls.resize( ndim );
  plotter.plot_slw.resize( ndim );

  unsigned nbin = stokes_crossed->get_nbin();
  
  unsigned jbin = bin;
  assert (jbin < nbin);

  unsigned idim=0;
  unsigned npol=4;

  for (unsigned ipol=0; ipol < npol; ipol++)
    for (unsigned jpol=0; jpol < npol; jpol++)
    {
      if (jpol == ipol)
      {
	Profile* profile = new Profile(nbin);
	plotter.profiles[idim] = profile;
	plotter.plot_sls[idim] = 1;
	plotter.plot_slw[idim] = 2;
	idim ++;
    
	float* amps = profile->get_amps();
	
	for (unsigned ibin=0; ibin < nbin; ibin++)
	{
	  Matrix<4,4,double> c = stokes_crossed->get_cross_covariance(ibin,jbin);
	  amps[ibin] = c[ipol][jpol];
	}
      }

      if (!splot_output)
	continue;
	 
      string fname = "xpol_" + tostring(ipol) + "_" + tostring(jpol) + ".dat";
      ofstream out (fname.c_str());

      int nbin2 = int(nbin)/2;
      
      for (unsigned ibin=0; ibin < nbin; ibin++)
      {
	for (int ilag=-nbin2; ilag<nbin2; ilag++)
	{
	  unsigned jbin = (int(ibin) + ilag + nbin) % nbin;
	  Matrix<4,4,double> c = stokes_crossed->get_cross_covariance(ibin,jbin);
	  out << ibin << " " << ilag << " " << c[ipol][jpol] << endl;
	}
	out << endl;
      }
    }
  
  assert (idim == ndim);
}

void Pulsar::StokesCrossCovariancePlot::plot_lags ()
{
  plotter.profiles.resize( lags+1 );
  plotter.plot_sls.resize( lags+1 );
  plotter.plot_slw.resize( lags+1 );

  unsigned nbin = stokes_crossed->get_nbin();
  
  for (unsigned ilag=0; ilag < lags+1; ilag++)
  {
    Profile* profile = new Profile(nbin);
    plotter.profiles[ilag] = profile;
    plotter.plot_sls[ilag] = 1;
    plotter.plot_slw[ilag] = 2;

    float* amps = profile->get_amps();

    for (unsigned ibin=0; ibin < nbin; ibin++)
    {
      unsigned jbin = (ibin + ilag) % nbin;
      Matrix<4,4,double> c = stokes_crossed->get_cross_covariance(ibin,jbin);
      amps[(ibin+ilag/2)%nbin] = c[covar.first][covar.second];
    }
  }
}

void Pulsar::StokesCrossCovariancePlot::plot_stats ()
{
  plotter.profiles.resize( 1 );
  plotter.plot_sls.resize( 1 );
  plotter.plot_slw.resize( 1 );

  unsigned nbin = stokes_crossed->get_nbin();
  Profile* profile = new Profile(nbin);
  float* amps = profile->get_amps();

  plotter.profiles[0] = profile;
  plotter.plot_sls[0] = 1;
  plotter.plot_slw[0] = 2;

  Reference::To<Profile> row = new Profile(nbin);
  float* ramps = row->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    ostringstream value;
    value << setfill('0') << setw(4) << ibin;
    string unload = string("bin_") + value.str() + string(".txt");

    ofstream out (unload.c_str());

    for (unsigned jbin=0; jbin < nbin; jbin++)
    {
      Matrix<4,4,double> c = stokes_crossed->get_cross_covariance(ibin,jbin);
      ramps[jbin] = c[covar.first][covar.second];
    }

    // ramps[ibin] = 0.5 * ( ramps[(ibin+1)%nbin] + ramps[(ibin+nbin-1)%nbin] );

    for (unsigned jbin=0; jbin < nbin; jbin++)
      out << jbin << " " << ramps[jbin] << endl;

    stats->set_Profile (row);
    string text = process( parser, expression );
    amps[ibin] = fromstring<float>( text );
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

  add( &StokesCrossCovariancePlot::get_expression,
       &StokesCrossCovariancePlot::set_expression,
       "exp", "Expression to evaluate for each row" );

  add( &StokesCrossCovariancePlot::get_config,
       &StokesCrossCovariancePlot::set_config,
       "config", "Configure the expression evaluator" );

  add( &StokesCrossCovariancePlot::get_splot_output,
       &StokesCrossCovariancePlot::set_splot_output,
       "splot", "produce text files for splot (bin>0 || max)");
}

