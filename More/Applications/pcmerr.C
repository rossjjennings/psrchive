/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"

#include "Pulsar/Archive.h"
#include "Pulsar/PolnCalibrator.h"

#include "Pauli.h"
#include "Stokes.h"

#include "strutil.h"

#include <cassert>

using namespace std;
using namespace Pulsar;

//
//! Analysis of error propagation in output of pcm
//
class pcmerr : public Pulsar::Application
{
public:

  //! Default constructor
  pcmerr ();

  //! Analyse PolnCalibrator from the given pcm output
  void process (Pulsar::Archive*);

protected:

  bool fscrunch;
  
  //! Add command line options
  void add_options (CommandLine::Menu&);
};



pcmerr::pcmerr ()
  : Application ("pcmerr", "pcm error propagation analysis")
{
  fscrunch = false;
}


void pcmerr::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // add a blank line and a header to the output of -h
  menu.add ("\n" "General options:");

  arg = menu.add (fscrunch, 'F');
  arg->set_help ("compute error after frequency integration");
}


void pcmerr::process (Pulsar::Archive* archive)
{
  Reference::To<Pulsar::PolnCalibrator> cal = new PolnCalibrator (archive);

  if (!cal->has_covariance ())
    throw Error (InvalidParam, "pcmerr::process",
		 "no model parameter covariance matrix");
      
  unsigned nchan = cal->get_nchan();

  const unsigned nparam = 7;    
  Matrix<nparam,nparam,double> covariance;

  unsigned ncov = nparam * (nparam+1) / 2;
  vector<double> cov (ncov);

  const unsigned npol = 4;

  Matrix<4,4,double> total_bias [npol];
  Stokes<double> total_Sprime [npol];
  for (unsigned i=0; i<npol; i++)
  {
    total_bias[i] *= 0.0;
    total_Sprime[i] *= 0.0;
  }

  unsigned count = 0;
  
  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    if (!cal->get_transformation_valid (ichan))
      continue;
    
    MEAL::Complex2* xform = cal->get_transformation(ichan);
    cal->get_covariance (ichan, cov);
    
    if (cov.size() != ncov)
      continue;

    // exract the 7x7 matrix of covariances between model parameters
    unsigned icov = 0;
    for (unsigned iparam=0; iparam < nparam; iparam++)
      for (unsigned jparam=iparam; jparam < nparam; jparam++)
	{
	  covariance[iparam][jparam] = cov[icov];
	  covariance[jparam][iparam] = cov[icov];
	  icov ++;
	}

    assert (icov == ncov);

    // cout << ichan << " " << covariance << endl;
    
    vector< Jones<double> > gradient;
    Jones<double> value = xform->evaluate (&gradient);
    assert (gradient.size() == nparam);

    Matrix<4,4,double> M = Mueller (value);
    Matrix<4,4,double> Minv = inv (M);

    // cout << ichan << " inv(M)=" << Minv << endl << endl;
 
    Matrix<4,4,double> max_bias = 0.0;
    max_bias *= 0.0;
    
    for (unsigned ipol=0; ipol < npol; ipol++)
    {
      Stokes<double> S (1.0,0,0,0);
      S[ipol] = 1.0;

      Stokes<double> Sprime = Minv * S;

      // cout << ichan << " S=" << Sprime << endl;
      
      Matrix<4,7,double> Jacobian = 0.0;
    
      for (unsigned iparam=0; iparam < nparam; iparam++)
      {
	Matrix<4,4,double> Mgrad = Mueller (value, gradient[iparam]);
	Matrix<4,4,double> Minvgrad = -Minv * Mgrad * Minv;

	// cout << ichan << " " << iparam << " " << Mgrad << endl << endl;
    
	Stokes<double> Sgrad = Minvgrad * S;
	
	for (unsigned jpol=0; jpol < npol; jpol++)
	  Jacobian[jpol][iparam] = Sgrad[jpol];
      }

      Matrix<4,4,double> bias = Jacobian * covariance * transpose(Jacobian);
      total_bias[ipol] += bias;
      total_Sprime[ipol] += Sprime;
      
      // bias relative to calibrated total intensity
      bias /= Sprime[0]*Sprime[0];
      
      for (unsigned jpol=0; jpol < npol; jpol++)
	for (unsigned kpol=0; kpol < npol; kpol++)
	  max_bias[jpol][kpol] = max( max_bias[jpol][kpol],
				      fabs(bias[jpol][kpol]) );
    }

    for (unsigned jpol=0; jpol < npol; jpol++)
      for (unsigned kpol=0; kpol < npol; kpol++)
	max_bias[jpol][kpol] = sqrt( max_bias[jpol][kpol] );

    count ++;

    if (!fscrunch)
      cout << "chan=" << ichan << " d=" << max_bias << endl;
  }

  if (!fscrunch)
    return;
  
  Matrix<4,4,double> max_bias = 0.0;
  max_bias *= 0;
  
  for (unsigned i=0; i<npol; i++)
  {
    total_bias[i] /= count;
    total_Sprime[i] /= count;

    // bias relative to calibrated total intensity
    total_bias[i] /= (total_Sprime[i] * total_Sprime[i]);

    for (unsigned jpol=0; jpol < npol; jpol++)
      for (unsigned kpol=0; kpol < npol; kpol++)
	max_bias[jpol][kpol] = max( max_bias[jpol][kpol],
				    fabs(total_bias[i][jpol][kpol]) );
  }

  for (unsigned jpol=0; jpol < npol; jpol++)
    for (unsigned kpol=0; kpol < npol; kpol++)
      max_bias[jpol][kpol] = sqrt( max_bias[jpol][kpol] );

  cout << max_bias << endl;
  
}

int main (int argc, char** argv)
{
  pcmerr program;
  return program.main (argc, argv);
}

