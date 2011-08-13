/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FourthMomentStats.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/StokesCovariance.h"
#include "Pulsar/ModeSeparation.h"

#include "Pauli.h"
#include "Jacobi.h"

using namespace std;

// #define _DEBUG

//! Default constructor
Pulsar::FourthMomentStats::FourthMomentStats (const PolnProfile* _profile)
  : PolnProfileStats (_profile)
{
  if (profile)
    covariance = profile->get_covariance();
}

//! Destructor
Pulsar::FourthMomentStats::~FourthMomentStats()
{
}

//! Set the FourthMoment from which statistics will be derived
void Pulsar::FourthMomentStats::set_profile (const PolnProfile* _profile)
{
  PolnProfileStats::set_profile (_profile);

  if (profile)
    covariance = profile->get_covariance();
}

void Pulsar::FourthMomentStats::eigen (PolnProfile& v1,
				       PolnProfile& v2,
				       PolnProfile& v3)
{
  if (!covariance)
    throw Error (InvalidState, "Pulsar::FourthMomentStats::eigen",
		 "covariance not set");

  PhaseWeight* baseline = stats->get_baseline ();
  Reference::To<StokesCovariance> clone = covariance->clone();
  for (unsigned imoment=0; imoment < StokesCovariance::nmoment; imoment++)
  {
    baseline->set_Profile( clone->get_Profile(imoment) );
    clone->get_Profile(imoment)->offset( -baseline->get_avg() );
  }
  covariance = clone;

  PolnProfile* v[3] = { &v1, &v2, &v3 };

  const unsigned nbin = profile->get_nbin();

  for (unsigned i=0; i<3; i++)
    v[i]->resize( nbin );

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    Matrix<4,4,double> C = covariance->get_covariance (ibin);

    Stokes<double> mean = profile->get_Stokes(ibin);
    double P = mean.abs_vect()/mean.get_scalar();
    double Pell = 0;

    if (P < 1 && P > 0)
      Pell = sqrt((1+P*P)/(1-P*P));
      
    Stokes<double> result;

    // get the eigen vectors of the coherency matrix
    Jones<double> rotation = convert( ::eigen( natural(mean) ) );
    Matrix<4,4,double> R = Mueller( rotation );

    // C = R * C * herm(R);

    double Ivar;
    Vector<3,double> Icovar;
    Matrix<3,3,double> pcovar;

    // extract the 3x3 covariance matrix of the polarization vector
    partition (C, Ivar, Icovar, pcovar);

    Vector<3,double> regression = inv(pcovar) * Icovar;
    double Rvar = regression * Icovar;
    regression /= norm(regression);

    Matrix<3,3,double> peigen;
    Vector<3,double> pvar;

    // compute the eigen vector matrix (a 3D rotation)
    Jacobi (pcovar, peigen, pvar);

    unsigned imax = 0;
    unsigned imin = 0;
    for (unsigned i=1; i<3; i++)
      if (pvar[i] > pvar[imax])
	imax = i;
      else if (pvar[i] < pvar[imin])
	imin = i;

    double Cell = 0;
    double CP = 0;
    if (pvar[imax] > 0 && pvar[imin] > 0)
    {
      Cell = pvar[imax]/pvar[imin];
      if (Cell > 1)
	CP = sqrt( (Cell-1)/(Cell+1) );
      Cell = sqrt( Cell );
    }

    Matrix<4,4,double> rotate;
    Vector<3,double> zero;

    // convert to a 4D rotation
    compose (rotate, 1.0, zero, peigen);

    C = rotate * C * herm(rotate);

    Vector<3,double> pmean = mean.get_vector();

    double cos_theta = regression * pmean / norm(pmean);
    double theta = acos (cos_theta);

#if 0
    if (theta > M_PI/2)
    {
      regression *= -1;
      theta = M_PI - theta;
    }
#endif

#if 1
    cout << ibin << " " << theta*180/M_PI 
	 << " " << Pell << " " << Cell
	 << " " << P << " " << CP << endl;
#else
    cout << ibin;
    for (unsigned i=0; i<3; i++)
      cout << " " << sqrt(pvar[i])/mean.get_scalar();
    cout << endl;
#endif

    for (unsigned i=0; i<4; i++)
      result[i] = C[0][i];

    // v[0]->set_Stokes (ibin, result);

    if (CP==0.0)
      regression = 0;

    Stokes<double> Rstokes (Ivar, Rvar*regression);

    v[0]->set_Stokes (ibin, R*Rstokes);

    //v[0]->set_Stokes (ibin, Stokes<double>(Ivar, peigen[imax]));
    //v[0]->set_Stokes (ibin, Stokes<double>(Ivar, peigen[imax]*pvar[imax]));


  }
}







void Pulsar::FourthMomentStats::separate (PolnProfile& modeA,
					  PolnProfile& modeB)
{
  if (!covariance)
    throw Error (InvalidState, "Pulsar::FourthMomentStats::separate",
		 "covariance not set");

  PhaseWeight* baseline = stats->get_baseline ();
  Reference::To<StokesCovariance> clone = covariance->clone();
  for (unsigned imoment=0; imoment < StokesCovariance::nmoment; imoment++)
  {
    baseline->set_Profile( clone->get_Profile(imoment) );
    cerr << "covar[" << imoment << "]=" << baseline->get_avg() << endl;
    clone->get_Profile(imoment)->offset( -baseline->get_avg() );
  }

  covariance = clone;

  const unsigned nbin = profile->get_nbin();

  modeA.resize( nbin );
  modeB.resize( nbin );

  ModeSeparation modes;

  float threshold = 3.0;

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    Stokes< Estimate<double> > stokes = get_stokes(ibin);
    if (stokes[0].get_value() < threshold * stokes[0].get_error())
      continue;

    cerr << "Pulsar::FourthMomentStats::separate ibin=" << ibin 
	 << " stokes=" << stokes << endl;

    modes.set_mean ( stokes );
    modes.set_covariance ( covariance->get_covariance (ibin) );

    modes.solve ();

    cerr << "modeA=" << coherency(modes.get_modeA()->evaluate()) << endl;
    cerr << "modeB=" << coherency(modes.get_modeB()->evaluate()) << endl;
    cerr << "modeC=" << coherency(modes.get_modeC()->evaluate()) << endl;
    cerr << "correlation=" << modes.get_correlation()->evaluate() << endl;
    cerr << "mean=" << coherency(modes.get_mean()->evaluate()) << endl;
  }
}
