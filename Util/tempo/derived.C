#include <math.h>

#include "ephio.h"
#include "psrephem.h"
#include "orbital.h"

#define sqr(x) (x*x)
#define cube(x) (x*x*x)

const double au   = 1.49597870e8;   // km
const double c    = 2.99792458e5;   // km/s
const double day  = 24.0 * 3600.0;  // seconds
const double year = 365.2422 * day; // seconds
const double mas  = M_PI/(60.0*60.0*180.0*1000.0);  // radians

// I.H. Stairs et al. 1998, ApJ 505:352-357 use T_sol = G * M_sol / c^3
// where G = Gravitational Constant
//       M = Mass of Sun
//       c = speed of light
const double T_sol  = 4.925490947e-6; // seconds


// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::mass_function (double& mf, double& mf_err) const
{
  mf_err = mf = -1.0;

  if (parmStatus[EPH_A1] < 1 || parmStatus[EPH_PB] < 1)
    return -1;

  double x     = value_double [EPH_A1];
  double x_err = error_double [EPH_A1];

  double pb = value_double [EPH_PB];
  double pb_err = error_double [EPH_PB];

  double n = 2.0 * M_PI / (pb * 86400.0);

  mf = sqr(n) * cube(x) / T_sol;
  mf_err = mf * sqrt( 9.0*sqr(x_err/x) + 4.0*sqr(pb_err/pb) );

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::sini (double& si, double& si_err) const
{
  if (parmStatus[EPH_KIN] > 0) {
    double i = value_double [EPH_KIN] * M_PI/180.0;
    si     = sin(i);
    si_err = cos(i) * error_double [EPH_KIN] * M_PI/180.0;
  }
  else if (parmStatus[EPH_SINI] > 0) {
    si     = value_double [EPH_SINI];
    si_err = error_double [EPH_SINI];
  }
  else
    return -1;

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
int psrephem::m1 (double& m1, double& m1_err) const
{
  if (parmStatus[EPH_M2] < 1 )
    return -1;

  double m2     = value_double [EPH_M2];
  double m2_err = error_double [EPH_M2];

  double si, si_err;
  if (sini (si, si_err) != 0)
    return -1;

  double mass_func, mass_func_err;
  if (mass_function (mass_func, mass_func_err) != 0)
    return -1;

  double term1 = sqrt (cube(m2*si)/mass_func);
  m1 = term1 - m2;
  m1_err = sqrt( sqr(0.5 * term1 * mass_func_err/mass_func) +
		 sqr(1.5 * term1 * si_err/si) +
		 sqr((1.5 * term1/m2 -1.0) * m2_err) );
  return 0;
}

int psrephem::m2 (double& m2, double m1) const
{
  double si, si_err;
  if (sini (si, si_err) != 0)
    return -1;

  double mf, mf_err;
  if (mass_function (mf, mf_err) != 0)
    return -1;

  m2 = companion_mass (mf, si, m1);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the composite proper motion
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::pm (double& pm, double& pm_err) const
{
  if (parmStatus[EPH_PMRA] < 1 || parmStatus[EPH_PMDEC] < 1)
    return -1;

  double covar = 0.0;   // covariance b/w mu_alpha and mu_delta

  double mu_alpha = value_double [EPH_PMRA];
  double mu_aerr  = error_double [EPH_PMRA];

  double mu_delta = value_double [EPH_PMDEC];
  double mu_derr  = error_double [EPH_PMDEC];

  pm = sqrt (sqr(mu_alpha) + sqr(mu_delta));

  pm_err = sqrt ( sqr(mu_alpha*mu_aerr) + sqr(mu_delta*mu_derr)
		  + 2.0 * covar * mu_alpha * mu_delta ) / pm;
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the proper motion celestial position angle
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::phi (double& phi, double& phi_err) const
{
  if (parmStatus[EPH_PMRA] < 1 || parmStatus[EPH_PMDEC] < 1)
    return -1;

  double covar = 0.0;   // covariance b/w mu_alpha and mu_delta

  double mu_alpha = value_double [EPH_PMRA];
  double mu_aerr  = error_double [EPH_PMRA];

  double mu_delta = value_double [EPH_PMDEC];
  double mu_derr  = error_double [EPH_PMDEC];

  double sqr_pm = sqr(mu_alpha) + sqr(mu_delta);

  phi = atan2 (mu_alpha, mu_delta);

  phi_err = sqrt ( sqr(mu_alpha*mu_derr) + sqr(mu_delta*mu_aerr)
		   + 2.0 * covar * mu_alpha * mu_delta ) / sqr_pm;
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the orbital period in seconds
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::P (double& p, double& p_err) const
{
  if (parmStatus[EPH_F] < 1)
    return -1;

  double rf = value_double [EPH_F];
  double rf_err = error_double [EPH_F];
  p = 1.0 / rf;
  p_err = p * rf_err / rf;

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the orbital period derivative
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::P_dot (double& p_dot, double& p_dot_err) const
{
  if (parmStatus[EPH_F] < 1 || parmStatus[EPH_F1] < 1)
    return -1;

  double rf = value_double [EPH_F];
  double rf_err = error_double [EPH_F];

  double rf_dot = value_double [EPH_F1];
  double rf_dot_err = error_double [EPH_F1];

  p_dot = - rf_dot / sqr(rf);
  p_dot_err = p_dot * sqrt(sqr(2*rf_err/rf) + sqr(rf_dot_err/rf_dot));

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::Shklovskii (double& beta, double& beta_err) const
{
  if (parmStatus[EPH_PX] < 1)
    return -1;

  double mu, mu_err;
  if (pm (mu, mu_err) < 0)
    return -1;

  double mu_radsec = mu * mas/year;

  double px = value_double [EPH_PX];
  double px_err = error_double [EPH_PX];

  // distance to pulsar in km
  double dist = au / (px * mas);
  double dist_err = dist * px_err/px;

  beta = sqr(mu_radsec) * dist / c;
  beta_err = beta * sqrt (sqr(2.0*mu_err/mu) + sqr(dist_err/dist));

  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_f_e (double& f_e) const
{
  f_e = 1.0;
  if (parmStatus[EPH_E] > 0) {
    double ecc2 = sqr(value_double [EPH_E]);
    f_e = (1.0 + 73.0/24.0*ecc2 + 37.0/96.0*sqr(ecc2)) / pow(1.0-ecc2,3.5);
  }
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the intrinsic x_dot due to gravitational wave emission
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_x_dot (double& x_dot) const
{
  if (parmStatus[EPH_A1] < 1)
    return -1;
  double x = value_double [EPH_A1];
  double x_err = error_double [EPH_A1];

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2     = value_double [EPH_M2];
  double m2_err = error_double [EPH_M2];

  double si, si_err;
  if (sini (si, si_err) < 0)
    return -1;

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;

  double f_e;
  GR_f_e (f_e);

  double fac  = T_sol / x;
  double totm = mp + m2;
  double sisq = sqr(si);

  // Kopeikin, 1996, ApJ 467:L93-L95 Eqn 15
  x_dot = -64.0/5.0 * cube(fac)*pow(si,4) * f_e * mp * pow(m2,5) / cube(totm);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the intrinsic Pb_dot due to gravitational wave emission
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_Pb_dot (double& Pb_dot) const
{
  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB];
  double pb_err = error_double [EPH_PB];

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2     = value_double [EPH_M2];
  double m2_err = error_double [EPH_M2];

  double f_e;
  GR_f_e (f_e);

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;
  
  double n = 2.0 * M_PI / (pb * 86400.0);
  
  // I.H. Stairs et al. 1998, ApJ 505:352-357 Eqn 10
  Pb_dot = - 192.0 * M_PI / 5.0 * pow (n*T_sol, 5.0/3.0) * f_e * mp * m2 
    / pow(mp+m2, 1.0/3.0);
  return 0;
}

// ////////////////////////////////////////////////////////////////////////
//
// returns the intrinsic omega_dot due to gravitational wave emission
// in degrees per year
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_omega_dot (double& w_dot) const
{
  double ecc2 = sqr(value_double [EPH_E]);

  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB];
  double pb_err = error_double [EPH_PB];

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2     = value_double [EPH_M2];
  double m2_err = error_double [EPH_M2];

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;

  double n = 2.0 * M_PI / (pb * 86400.0);

  // I.H. Stairs et al. 1998, ApJ 505:352-357 Eqn 8
  w_dot = 3.0 * pow(n, 5.0/3.0) * pow((mp+m2)*T_sol, 2.0/3.0) / (1.0-ecc2);

  w_dot *= year * 180.0 / M_PI;
  return 0;
}


// ////////////////////////////////////////////////////////////////////////
//
// returns the general relativistic prediction of gamma
//
// ////////////////////////////////////////////////////////////////////////
int psrephem::GR_gamma (double& gamma) const
{
  double ecc = value_double [EPH_E];

  if (parmStatus[EPH_PB] < 1)
    return -1;
  double pb = value_double [EPH_PB];
  double pb_err = error_double [EPH_PB];

  if (parmStatus[EPH_M2] < 1 )
    return -1;
  double m2     = value_double [EPH_M2];
  double m2_err = error_double [EPH_M2];

  double mp, mp_err;
  if (m1 (mp, mp_err) < 0)
    return -1;

  double n = 2.0 * M_PI / (pb * 86400.0);

  // I.H. Stairs et al. 1998, ApJ 505:352-357 Eqn 3
  double cube_root = sqr(T_sol) / (n*pow(mp+m2,4.0));

  gamma = ecc * pow(sqr(T_sol)/(n*pow(mp+m2,4.0)), 1.0/3.0) * m2*(mp + 2.0*m2);

  return 0;
}

