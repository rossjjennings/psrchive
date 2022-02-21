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

  unsigned npol = 4;
  
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

    vector< Jones<double> > gradient;
    Jones<double> value = xform->evaluate (&gradient);
    assert (gradient.size() == nparam);

    Matrix<4,4,double> M = Mueller (value);

    Vector<4,double> diagonal = 0.0;
    diagonal *= 0.0;
    
    for (unsigned ipol=0; ipol < npol; ipol++)
    {
      Stokes<double> S (0,0,0,0);
      S[ipol] = 1.0;
      
      Matrix<4,7,double> Jacobian = 0.0;
    
      for (unsigned iparam=0; iparam < nparam; iparam++)
      {
	Matrix<4,4,double> Mgrad = Mueller (value, gradient[iparam]);
	Matrix<4,4,double> Minv = inv (M);
	Matrix<4,4,double> Minvgrad = -Minv * Mgrad * Minv;
	
	Stokes<double> Sgrad = Minvgrad * S;
	
	for (unsigned jpol=0; jpol < npol; jpol++)
	  Jacobian[jpol][iparam] = Sgrad[jpol];
      }

      Matrix<4,4,double> bias = Jacobian * covariance * transpose(Jacobian);
      for (unsigned jpol=0; jpol < npol; jpol++)
	diagonal[jpol] += bias[jpol][jpol];
    }

    for (unsigned jpol=0; jpol < npol; jpol++)
      diagonal[jpol] = sqrt( diagonal[jpol] );
    
    cout << ichan << " " << diagonal << endl;
  }  
}

int main (int argc, char** argv)
{
  pcmerr program;
  return program.main (argc, argv);
}

