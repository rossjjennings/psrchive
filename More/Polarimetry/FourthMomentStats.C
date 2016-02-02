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
#include "Minkowski.h"

using namespace std;

// #define _DEBUG

//! Default constructor
Pulsar::FourthMomentStats::FourthMomentStats (const PolnProfile* _profile)
  : PolnProfileStats (_profile)
{
  if (profile)
    covariance = profile->get_covariance();

  duration = folding_period = bandwidth = 0.0;
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

void Pulsar::FourthMomentStats::set_bandwidth (double bw)
{
  bandwidth = fabs(bw);
}


void sort (Vector<3,double>& v)
{
  if (v[0] < v[1])
    std::swap (v[0], v[1]);
  if (v[1] < v[2])
    std::swap (v[1], v[2]);
  if (v[0] < v[1])
    std::swap (v[0], v[1]);
}

Reference::To<Pulsar::Profile> Pulsar::FourthMomentStats::get_modulation_index()
{
  if (folding_period <= 0)
    throw Error (InvalidState,
		 "Pulsar::FourthMomentStats::get_modulation_index", 
		 "folding period unknown");

  if (bandwidth <= 0)
    throw Error (InvalidState,
		 "Pulsar::FourthMomentStats::get_modulation_index",
		 "bandwidth unknown");

  if (duration <= 0)
    throw Error (InvalidState,
		 "Pulsar::FourthMomentStats::get_modulation_index",
		 "integration length unknown");

  const unsigned nbin = covariance->get_nbin();

  Reference::To<Profile> result = covariance->get_Profile(0)->clone();

  PhaseWeight* baseline = stats->get_baseline ();
  double off_pulse_moment;
  baseline->stats (result, &off_pulse_moment);

  Stokes<double> off_pulse_mean;
  Stokes<double> off_pulse_var;
  for (unsigned i=0; i<4; i++)
  {
    const Profile* p = profile->get_Profile(i);
    baseline->stats (p, &off_pulse_mean[i], &off_pulse_var[i]);
  }

  const float* I = profile->get_Profile(0)->get_amps();

  cerr << "bandwidth=" << bandwidth
       << " weight=" << profile->get_Profile(0)->get_weight() << endl;

#define ASSUME_EFFECTIVE_BANDWIDTH 0
#if ASSUME_EFFECTIVE_BANDWIDTH

  /*
    the following code computes the effective banwidth of CASPSR
    after bad channels have been zapped
  */

  // 62 is the number of sub-integrations in first.meta

  double effective_nchan = profile->get_Profile(0)->get_weight () / 62;

  cerr << "effective nchan=" << effective_nchan << endl;

  double original_nchan = 512;

  bandwidth *= effective_nchan / original_nchan;

  cerr << "effective bandwidth=" << bandwidth << endl;

#endif

  double npulse = duration / folding_period;
  double nsample = folding_period * bandwidth * 1e6 / nbin;

  cerr << "npulse=" << npulse << " nsample=" << nsample << endl;

  double fnorm = 1.0;

  double ratio = off_pulse_var[0] / off_pulse_moment;
  if (ratio > 4 || ratio < 0.25)
  {
    // this is approx. the normalization factor that psr4th should have applied
    fnorm = npulse / profile->get_Profile(0)->get_weight();
    cerr << "correcting psr4th bug by norm=" << fnorm << endl;
  }

  cerr << "mean=" << off_pulse_mean[0] << " var=" << off_pulse_var[0]
       << " moment=" << off_pulse_moment
       << " normalized moment=" << off_pulse_moment / fnorm << endl;

  cerr << "missing factor=" << off_pulse_var[0] / (off_pulse_moment / fnorm) << endl;

  double dof = nsample * 2;

  double predicted = normsq(off_pulse_mean)/dof;
  double measured = off_pulse_moment*npulse/fnorm;

#define BASEBAND_FOURTH 0
#if BASEBAND_FOURTH
  predicted =  normsq(off_pulse_mean);
  measured = off_pulse_moment;
#endif

  cerr << "dof=" << dof << endl;
  cerr << "predicted rms=" << sqrt(predicted)
       << endl
       << "measured rms=" << sqrt(measured)
       << endl;
  
  double correction = measured / predicted;

  cerr << "correction=" << correction << endl;

  float* M = result->get_amps();

  double peak_bias = 0;

  for (unsigned i=0; i<nbin; i++)
  {
    float Ival = I[i] - off_pulse_mean[0];
    double Isq = Ival * Ival;

    if (Isq < 100 * off_pulse_var[0])
      M[i] = 0;
    else
    {
      double bias = normsq( profile->get_Stokes(i) );
      //- normsq( profile->get_Stokes(i) - off_pulse_mean )
      //- normsq( off_pulse_mean );

      bias /= dof;

      // bias = 0;

      // bias *= correction;
#if 0
      cerr << "bias=" << bias
	   << " off=" << off_pulse_moment*npulse/fnorm << endl;
#endif

      peak_bias = std::max (bias, peak_bias);
	
      M[i] = (M[i]*npulse/fnorm - bias) / Isq;
      if (M[i] < 0)
	M[i] = 0;
      else
	M[i] = sqrt(M[i]);
    }

  }

  cerr << "peak bias=" << peak_bias
       << " off=" << off_pulse_moment*npulse/fnorm << endl;

  return result;
}

void Pulsar::FourthMomentStats::eigen (PolnProfile* v1,
				       PolnProfile* v2,
				       PolnProfile* v3)
{
  if (!covariance)
    throw Error (InvalidState, "Pulsar::FourthMomentStats::eigen",
		 "covariance not set");

  PhaseWeight* baseline = stats->get_baseline ();
  Reference::To<StokesCovariance> clone = covariance->clone();

  vector<double> baseline_mean_moment (StokesCovariance::nmoment);

  for (unsigned imoment=0; imoment < StokesCovariance::nmoment; imoment++)
  {
    Profile* profile = clone->get_Profile(imoment);
    baseline->set_Profile( profile );

    double mean, variance;
    baseline->stats (profile, &mean, &variance);
    
    baseline_mean_moment[imoment] = mean;

    // profile->offset( -mean );
  }
  covariance = clone;

  const unsigned npol = 4;
  unsigned diagonal [npol] = { 0, 4, 7, 9 };

  Vector<npol,double> baseline_mean_pol;
  for (unsigned ipol=0; ipol < npol; ipol++)
  {
    const Profile* prof = profile->get_Profile(ipol);
    baseline->set_Profile( prof );

    double mean, variance;
    baseline->stats (prof, &mean, &variance);
    
    baseline_mean_pol[ipol] = mean;

    cout << "baseline mean=" << mean << " var=" << variance << " vs " 
	 << baseline_mean_moment[ diagonal[ipol] ] << endl;
  }

  Matrix<4,4,double> expected = Minkowski::outer(baseline_mean_pol,baseline_mean_pol);

  unsigned k=0;
  for (unsigned i=0; i<4; i++)
    for (unsigned j=i; j<4; j++)
      {
	cerr << "expected[" << i << "][" << j << "]=" << expected[i][j] << endl;
	cerr << "dof=" << expected[i][j] / baseline_mean_moment[k] << endl;
	k++;
      }

  PolnProfile* v[3] = { v1, v2, v3 };

  const unsigned nbin = profile->get_nbin();

  eigen_value.resize(3);
  regression_coefficient.resize(3);
  natural_covariance.resize(3);

  for (unsigned i=0; i<3; i++)
  {
    if (v[i])
      v[i]->resize( nbin );

    eigen_value[i] = new Profile (nbin);
    regression_coefficient[i] = new Profile (nbin);
    natural_covariance[i] = new Profile (nbin);
  }

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

    Matrix<3,3,double> peigen;
    Vector<3,double> pvar;

    // compute the eigen vector matrix (a 3D rotation)
    Jacobi (pcovar, peigen, pvar);

    Vector<3,double> natural = peigen * Icovar;

#if 0
    for (unsigned i=0; i<3; i++)
      if (peigen[i] * mean.get_vector() < 0)
	peigen[i] *= -1.0;
#endif

#define METRIC(i) (pvar[i] + fabs(natural[i]))

    // sort the eigen values from greatest to least
    unsigned order [3] = { 0, 1, 2 };
    for (unsigned j=0; j<2; j++)
      for (unsigned i=j+1; i<3; i++)
	if ( METRIC(order[i]) > METRIC(order[j]) )
	  std::swap (order[i], order[j]);

    for (unsigned i=0; i < 3; i++)
    {
      eigen_value[i]->get_amps()[ibin] = pvar[order[i]];
      natural_covariance[i]->get_amps()[ibin] = fabs(natural[order[i]]);
      regression_coefficient[i]->get_amps()[ibin] = regression[i];
    }

    double Rvar = regression * Icovar;
    regression /= norm(regression);


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

#if 0
    double cos_theta = regression * pmean / norm(pmean);
    double theta = acos (cos_theta);

    if (theta > M_PI/2)
    {
      regression *= -1;
      theta = M_PI - theta;
    }
#endif

#if 0
    cout << ibin << " " << theta*180/M_PI 
	 << " " << Pell << " " << Cell
	 << " " << P << " " << CP << endl;
//#else
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

    // v[0]->set_Stokes (ibin, R*Rstokes);

    //v[0]->set_Stokes (ibin, Stokes<double>(Ivar, peigen[imax]));
    //v[0]->set_Stokes (ibin, Stokes<double>(Ivar, peigen[imax]*pvar[imax]));


  }
}



//! Get the Stokes parameters of the specified phase bin
Matrix< 4,4,Estimate<double> >
Pulsar::FourthMomentStats::get_covariance (unsigned ibin) const try
{
  Matrix< 4,4,Estimate<double> > result = covariance->get_covariance (ibin);

  unsigned imoment = 0;
  for (unsigned ipol=0; ipol < 4; ipol++)
    for (unsigned jpol=ipol; jpol < 4; jpol++)
    {
      double var = get_moment_variance(imoment).get_value(); imoment ++;

      result[ipol][jpol].set_variance( var );
      result[jpol][ipol].set_variance( var );
    }

  return result;
}
 catch (Error& error)
   {
     throw error += "Pulsar::FourthMomentStats::get_covariance";
   }


Estimate<double>
Pulsar::FourthMomentStats::get_moment_variance (unsigned imom) const try
{
  if (imom >= StokesCovariance::nmoment)
    throw Error (InvalidParam,
		 "Pulsar::FourthMomentStats::get_moment_variance",
		 "imoment=%u >= nmoment=%u",
		 imom, StokesCovariance::nmoment);

  moment_variance.resize( StokesCovariance::nmoment );

  if (moment_variance[imom].get_value() == 0)
  {
    stats->set_profile( covariance->get_Profile(imom) );
    moment_variance[imom] = stats->get_baseline_variance();
#ifdef _DEBUG
    cerr << "Pulsar::FourthMomentStats::get_moment_variance imom="
         << imom << " var=" << moment_variance[imom] << endl;
#endif
  }
  return moment_variance[imom];
}
 catch (Error& error)
   {
     throw error += "Pulsar::FourthMomentStats::get_moment_variance";
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

  float threshold = 10.0;

  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    Stokes< Estimate<double> > stokes = get_stokes(ibin);
    if (stokes[0].get_value() < threshold * stokes[0].get_error())
      continue;

    cerr << "Pulsar::FourthMomentStats::separate ibin=" << ibin << endl
	 << "stokes=" << stokes << endl;

    modes.set_mean ( stokes );
    modes.set_covariance ( get_covariance (ibin) );

    modes.solve ();

    cerr << "modeA=" << coherency(modes.get_modeA()->evaluate()) << endl;
    cerr << "modeB=" << coherency(modes.get_modeB()->evaluate()) << endl;
    cerr << "modeC=" << coherency(modes.get_modeC()->evaluate()) << endl;
    cerr << "correlation=" << modes.get_correlation()->evaluate() << endl;
    cerr << "mean=" << coherency(modes.get_mean()->evaluate()) << endl;
  }
}
