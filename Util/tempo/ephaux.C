/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <iostream>
#include <math.h>
#include <unistd.h>

#include "psrephem.h"
#include "ephio.h"

using namespace std;
using Legacy::psrephem;

void usage()
{
  cout << "\nephaux: print stuff about the pulsar from various theories"
    "\n"
    "  -v        verbose\n"
       << endl;
}

#define sqr(x) (x*x)
#define cube(x) (x*x*x)

const double day    = 24.0 * 3600.0;  // seconds
const double year   = 365.2422 * day; // seconds
const double au     = 1.49597870e8;     // km

// Gaussian Gravitational Constant
const double gGc    = 0.01720209895;  // AU^3/2 / (day* M_s^1/2)
const double GM_s   = sqr(gGc/day) * cube(au);  // km^3 / (s^2 * M_s)
const double GM_so  = 1.3271243999e26;

const double c      = 2.99792458e5;   // km/s
const double parsec = 3.085678e13;    // km
const double mas    = M_PI/(60.0*60.0*180.0*1000.0);  // radians
const double deg    = 180.0/M_PI;

int main (int argc, char ** argv)
{
  bool verbose = false;
  int gotc = 0;

  double dist_dot = 100.0;  // km/s

  float efac = 0.0;

  while ((gotc = getopt(argc, argv, "d:e:hv")) != -1) {
    switch (gotc) {

    case 'd':
      dist_dot = atof (optarg);
      break;

    case 'e':
      efac = atof (optarg);
      break;

    case 'h':
      usage ();
      return 0;

    case 'v':
      verbose = true;
      break;
    }
  }

  if (optind >= argc) {
    cerr << "Please provide pulsar .par as the last argument" << endl;
    return -1;
  }

  psrephem eph (argv[optind]);
  if (efac > 0)
    eph.efac(efac);

  cerr << "1" << endl;

  // ephio parms of interest
  // EPH_RAJ    - alpha     - right ascension in turns
  // EPH_DECJ   - delta     - declination in turns

  // EPH_PMDEC  - mu_delta  - proper motion in declination (mas/yr)
  // EPH_PMRA   - mu_alpha  - proper motion in right ascension (mas/yr)

  // EPH_PX     -           - parallax in mas
  // EPH_KOM   - big Omega - longitude of ascending node

  // psrephem has parmStatus and value_double and error_double

  double mu_alpha = eph.value_double [EPH_PMRA];
  double mu_aerr  = eph.error_double [EPH_PMRA];

  double mu_delta = eph.value_double [EPH_PMDEC];
  double mu_derr  = eph.error_double [EPH_PMDEC];

  // composite proper motion
  double pm = sqrt (sqr(mu_alpha) + sqr(mu_delta));
  double mu_aesq = mu_aerr * mu_aerr;
  double mu_desq = mu_derr * mu_derr;
  double pm_err = sqrt(sqr(mu_alpha)*mu_aesq + sqr(mu_delta)*mu_desq)/pm;
  double pm_f = pm_err / pm;

  double phi = atan2 (mu_alpha, mu_delta);
  double phi_err = sqrt(sqr(mu_alpha)*mu_desq + sqr(mu_delta)*mu_aesq)/(pm*pm);
  cerr << "1.5" << endl;

  double px = eph.value_double [EPH_PX];
  double px_err = eph.error_double [EPH_PX];

  double pb = eph.value_double [EPH_PB];
  double pb_err = eph.error_double [EPH_PB];
  double pb_f = pb_err / pb;

  double pb_dot=0, pb_dot_err=0, pb_dot_f=0;

  if (eph.parmStatus[EPH_PBDOT] > 0) {
    pb_dot = eph.value_double [EPH_PBDOT] * 1e-12;
    pb_dot_err = eph.error_double [EPH_PBDOT] * 1e-12;
    pb_dot_f = pb_dot_err / pb_dot;
  }

  // distance to pulsar in km
  double dist = au / (px * mas);
  double dist_err = dist * px_err/px;

  // cout << "dist in parsec = " << 1e3/px << endl;

  cerr << "2" << endl;

  double mass_func, mass_func_err;
  eph.mass_function (mass_func, mass_func_err);
  cout << "mass funtion = " << mass_func << endl;

  double l=0,b=0;
  eph.galactic (l, b);
  cerr << "sin(b)=" << sin(b) << endl;

  double acc=0;
  eph.acc_diffrot (acc);
  cerr << "g_R=" << acc << "km/s^2;  beta dr=" << acc/c << "s-1" << endl;

  double z=0;
  eph.galactic_z(z);
  cerr << "z(pc) = " << z << endl;

  double unit = 4.0; // Fig. 7 - distance of 550 pc
  double Kz = unit * 1.65e-14;
  cerr << "Kz = " << Kz << " km/s^2" << endl;
  cerr << "beta kz=" << -Kz*sin(b)/c << " s-1" << endl;

  cout <<
    "*******************************************************************\n"
    "Working from Shklovskii's pm-induced pdot    **********************\n\n"
    "Using:\n"
    " pm_ra  = " << mu_alpha << " \261 " << mu_aerr << " mas/yr\n"
    " pm_dec = " << mu_delta << " \261 " << mu_derr << " mas/yr\n"
    " -> pm_tot = " << pm << " \261 " << pm_err  << " mas/yr\n"
    "    pm_phi = " << phi*deg << " \261 " << phi_err  << " deg\n"
    "          l=" << l*180/M_PI << " b=" << b*180/M_PI << "\n"
    " Pb     = " << pb << " \261 " << pb_err << " days\n"
    " Pb-dot = " << pb_dot << " \261 " << pb_dot_err << "\n" << endl;


  double pb_r = pb_dot / (pb * 86400.0);
  double pb_r_err = pb_r * sqrt (pb_f*pb_f + pb_dot_f*pb_dot_f);
  double pb_r_f = pb_r_err / pb_r;

  // now:  pb_r = pb_dot/pb = pm * pm * dist /c
  // -> dist = c * pb_r / (pm*pm)

  double pm_radsec = pm * mas / year;
  double Sdist = c * pb_r / sqr(pm_radsec);
  double Sdist_err = Sdist * sqrt (sqr(pb_r_f) + sqr(2.0*pm_f));

  cout <<
    "-> Shklovskii distance = " << Sdist/parsec <<" \261 "
       << Sdist_err/parsec << " parsec\n\n"

    "Compare with: \n"
    " px     = " << px << " \261 " << px_err << " mas\n\n"
    "-> Parallax distance = " << dist/parsec <<" \261 "
       << dist_err/parsec << " parsec\n"
    "-> Tangential velocity " << dist * pm_radsec << " km/s\n"
       << endl;

  // kinematic contribution to period derivatives (Shklovskii)
  double beta, beta_err;
  cerr << "2.5" << endl;
  if (eph.quadratic_Doppler (beta, beta_err) != 0)
    cerr << "ephaux: could not calculate Shklovskii effect" << endl;

  cerr << " Quadratic Doppler = " << beta << " \261 " << beta_err << endl;

  double SPb_dot = beta * (pb * 86400.0);
  double SPb_dot_err = SPb_dot * sqrt (sqr(beta_err/beta) + sqr(pb_err/pb));

  cerr << "3" << endl;
  cout << 
    "Alternatively, use the parallax-derived distance and proper motion\n"
    "to derive limits on the orbital period derivative:\n\n"
    "-> S.Pb-dot = " << SPb_dot << " \261 " << SPb_dot_err << "\n" << endl;

  // cout << "-> Shklovskii = " << Shklovskii << " s-1\n" << endl;

  double V = 20.62;
  double V_err = 0.1;

  double VR = 0.71;
  double VR_err = 0.03;

  double RI = 0.51;
  double RI_err = 0.04;

  double I = V - VR - RI;
  double I_err = V_err + VR_err + RI_err;

  double MI = I - 5.0 * log10 (1e2/px);
  double MI_err = 2.5 * log10 ((px+px_err)/(px-px_err)) + I_err;

  cerr << "Absolute magnitude MI " << MI << " +- " << MI_err << endl;

  cout << endl <<
    "*******************************************************************\n"
    "Now pm-induced p_dot_dot.  Assuming a radial velocity of: "
       << dist_dot << " km/s and ..."  << endl;

  double p, p_err;
  eph.P(p,p_err);
  double p_dot, p_dot_err;
  eph.P_dot(p_dot,p_dot_err);
  double p_ddot, p_ddot_err;
  eph.P_ddot(p_ddot,p_ddot_err);

  double Ski_P_dot = beta * p;

  cout <<
    " P     = " << p     << " \261 " << p_err  << " s\n"
    " Pdot  = " << p_dot << " \261 " << p_dot_err  << "\n"
    " Pddot = " << p_ddot << " \261 " << p_ddot_err  << "\n"
       << endl;

  double gamma = 1.0 + dist_dot / c;

  double p0 = p / gamma;
  double p0_dot = (p_dot - Ski_P_dot) / gamma;

  double beta_P_dot_dot = 2.0 * p0_dot * beta - 3.0 * p0 * dist_dot * beta/dist;

  cout <<
    "-> S.P_dot_dot = " << beta_P_dot_dot << " s^-1\n" << endl;

  double err = 0;
  eph.Doppler_P_dotdot (beta_P_dot_dot, err, dist_dot, 0);
  cout <<
    "-> S.P_dot_dot2 = " << beta_P_dot_dot << " s^-1\n" << endl;

  double f2, f2_err;
  eph.Doppler_F2 (f2, f2_err, dist_dot, 0);
  cout <<
    "-> Doppler F2 = " << f2 << " s^-3\n" << endl;

  double phi_min = 50e-9 / p;
  double t_min = cbrt ( 6.0 * phi_min / f2 );
  cout <<
    "-> Doppler F2 min T = " << t_min/3600.0/24.0/365.25 << " years\n" << endl;

  double int_P_dot_dot, int_P_dot_dot_err;
  eph.intrinsic_P_dotdot (int_P_dot_dot, int_P_dot_dot_err);
  cout <<
    "-> intrinsic P_dot_dot = " << int_P_dot_dot << " s^-1\n" << endl;


  double bad_vel = 2.0*p_dot*dist / (3.0*p);

  cout << " Bad velocity = " << bad_vel << endl;

  double p_dot_int = p_dot - p * beta;
  double p_dot_int_err = p_dot_err 
    + p * beta * sqrt(sqr(p_err/p)+sqr(beta_err/beta));

  double char_age = 0.5 * p / p_dot_int / (86400.0e9 * 365.25);
  double char_age_err = char_age
    * sqrt(sqr(p_err/p)+sqr(p_dot_int_err/p_dot_int));

  double wd_age1 = 4.0;
  double wd_age2 = 4.3;
  double original_P1 = sqrt(1.0 - wd_age1/(char_age+char_age_err)) * p;
  double original_P2 = sqrt(1.0 - wd_age2/(char_age-char_age_err)) * p;

  cerr << "3" << endl;
  cerr << endl <<
    "*******************************************************************\n"
    "Subtract Pdot due to proper motion at parallax distance:\n"
    "  S.P-dot = " << beta*p << "\n"
    "\n"
    "-> characteristic age = " << char_age << " pm " << char_age_err << " Gyr\n"

    "He WD age of " << wd_age1 << " -> " << wd_age2 << " Gyr\n"
    "-> P_orig = " << original_P1 << " -> " << original_P2 << endl;

  double x     = eph.value_double [EPH_A1];
  double x_err = eph.error_double [EPH_A1];
  double x_f   = x_err / x;
    
  double xdot     = eph.value_double [EPH_XDOT] * 1e-12;
  double xdot_err = eph.error_double [EPH_XDOT] * 1e-12;

  double sini, sini_err;

  if (eph.parmStatus[EPH_KIN] > 0) {
    double i = eph.value_double [EPH_KIN] * M_PI/180.0;
    sini     = sin(i);
    sini_err = cos(i) * eph.error_double [EPH_KIN] * M_PI/180.0;

#if 0
    double i1 = eph.value_double [EPH_KIN] + eph.error_double [EPH_KIN];
    double i2 = eph.value_double [EPH_KIN] - eph.error_double [EPH_KIN];
    cerr << "sini_1=" << sin(i1*M_PI/180.0) << endl;
    cerr << "sini_2=" << sin(i2*M_PI/180.0) << endl;
#endif

  }
  else {
    sini     = eph.value_double [EPH_SINI];
    sini_err = eph.error_double [EPH_SINI];
  }
  cerr << "4" << endl;

  if (eph.value_integer [EPH_K96] != 1) {
    cout << endl <<
      "*******************************************************************\n"
      "Working from Kopeikin, 1996, ApJ 467:L93-L95 **********************\n"
	 << endl;
    
    // From Kopeikin, 1996, ApJ, 467, L93.  Equation 11
    // constraint on i from xdot, greatest when ddx_dOmega = 0, or
    double Omega = atan2 (-mu_alpha, mu_delta);
    double sOmega = sin(Omega);
    double cOmega = cos(Omega);
    
    double orbi = asin (sini);
    double tani = tan (orbi);
    
    /*
      for (double om = 0; om < 360; om += 5) {
      double som = sind (om);
      double com = cosd (om);
      double poop = mas/year * x/tani * (-mu_alpha*som + mu_delta*com);
      cout << om << " " << poop << endl;
      }
      */
    
    double pm_xdot = mas/year * x/tani * (-mu_alpha*sOmega + mu_delta*cOmega);
    
    cout << "Using: \n"
      " x      = " << x << " \261 " << x_err << " s\n"
      " sini   = " << sini << " \261 " << sini_err << "\n"
      " Omega  = " << Omega * 180.0 / M_PI << "\n\n"
      
      "-> x-dot < " << pm_xdot << "\n\n"
      
      "Compare with measured value: \n"
      "   x-dot = " << xdot << " \261 " << xdot_err << "\n"
	 << endl;

    double tani_max = pm_xdot * tani / xdot;
    double i_max = atan(tani_max);
    
    cout << "Alternatively, use the above value for xdot and limit i\n"
      "-> tan(i) < " << tani_max << "\n"
      "       i  < " << i_max * 180.0 / M_PI << " deg\n"
      "   sin(i) < " << sin(i_max) << endl << endl;
    
    /*
      for (double om = 0; om < 360; om += 2) {
      double Omega_err = 3.5 * M_PI/180.0;
      sOmega = sin(om * M_PI/180.0);
      cOmega = cos(om * M_PI/180.0);
      
      double term1 = x/xdot * mas/year;
      double tani = term1 * (-mu_alpha*sOmega + mu_delta*cOmega);
      
      double dtani_dx = tani / x;
      double dtani_dxdot = -tani / xdot;
      double dtani_dOmega = term1 * (-mu_alpha*cOmega - mu_delta*sOmega);
      double dtani_dmu_alpha = -term1 * sOmega;
      double dtani_dmu_delta = term1 * cOmega;
      
      double tani_err = sqrt ( sqr(dtani_dx) * sqr(x_err) +
      sqr(dtani_dxdot) * sqr(xdot_err) +
      sqr(dtani_dOmega) * sqr(Omega_err) +
      sqr(dtani_dmu_alpha) * sqr(mu_aerr) +
      sqr(dtani_dmu_delta) * sqr(mu_derr) );
      
      cout << om << " " << dtani_dOmega << endl;
      }
      */
    
    cerr << "5" << endl;
    if (eph.parmStatus[EPH_KOM] > 0) {
      
      // considering the prefered Omega from the par file
      Omega = eph.value_double [EPH_KOM];
      double Omega_err = eph.error_double [EPH_KOM];

      cout <<
	"Or, using the value of Omega from the .par file:\n"
	" Omega  = " << Omega << " \261 " << Omega_err << " deg\n";
      
      Omega *= M_PI / 180.0;
      Omega_err *= M_PI / 180.0;
      
      sOmega = sin(Omega);
      cOmega = cos(Omega);

      double term1 = x/xdot * mas/year;
      double tani = term1 * (-mu_alpha*sOmega + mu_delta*cOmega);

      double dtani_dx = tani / x;
      double dtani_dxdot = -tani / xdot;
      double dtani_dOmega = term1 * (-mu_alpha*cOmega - mu_delta*sOmega);
      double dtani_dmu_alpha = -term1 * sOmega;
      double dtani_dmu_delta = term1 * cOmega;
      
      double tani_err = sqrt ( sqr(dtani_dx) * sqr(x_err) +
			       sqr(dtani_dxdot) * sqr(xdot_err) +
			       sqr(dtani_dOmega) * sqr(Omega_err) +
			       sqr(dtani_dmu_alpha) * sqr(mu_aerr) +
			       sqr(dtani_dmu_delta) * sqr(mu_derr) );
      
      double imid = atan(tani);
      double cosi = cos(imid);
      sini = sin(imid);
      
      double dsini_dtani = cube(cosi);
      sini_err = dsini_dtani * tani_err;
    
      double i_err = sqr(cosi) * tani_err;

      cout << 
	"-> tan(i) = " << tani << " \261 " << tani_err << "\n"
	"   sin(i) = " << sini << " \261 " << sini_err << "\n"
	"   i      = " << imid*deg << " \261 " << i_err*deg << "\n" << endl;
    }
  }

  cerr << "6" << endl;
  
  if (eph.parmStatus[EPH_KOM] > 0) {

    // considering the prefered Omega from the par file
    double Omega = eph.value_double [EPH_KOM];
    double Omega_err = eph.error_double [EPH_KOM];
    
    cout <<
      "*******************************************************************\n"
      "Using the value of Omega from the .par file:\n"
      " Omega  = " << Omega << " \261 " << Omega_err << " deg\n";
      
    Omega *= M_PI / 180.0;
    Omega_err *= M_PI / 180.0;

    double pm_xdot, pm_xdot_err;
    eph.pm_x_dot (pm_xdot, pm_xdot_err);

    double pm_omdot, pm_omdot_err;
    eph.pm_omega_dot (pm_omdot, pm_omdot_err);

    cout << 
      "proper motion contribution to xdot " << pm_xdot << "\n"
      "proper motion contribution to omdot " << pm_omdot << endl;

    double sOmega = sin(Omega);
    double cOmega = cos(Omega);
    
    // ///////////////////////////////////////////////////////////////////
    // From Kopeikin, 1996, ApJ, 467, L93.  Equation 9

    double mu_dot = - 2.0 * pm * dist_dot * year / dist; // mas/yr^2

    double mu_alpha_dot = sin(phi) * mu_dot * mas / sqr(year); // rad/s^2
    double mu_delta_dot = cos(phi) * mu_dot * mas / sqr(year); // rad/s^2

    double pb_sec = pb * 86400.0;

    double wpb_dot_max = 0.0;

    double i_dot = (-mu_alpha * sOmega + mu_delta * cOmega) * mas/year;
    double w_dot = (mu_alpha * cOmega + mu_delta * sOmega)/sini * mas/year;

    double dw_dot_dmu = (mu_alpha_dot * cOmega + mu_delta_dot * sOmega)/sini;

    double orbi = asin (sini);
    double tani = tan (orbi);
    double B_dot = -i_dot*w_dot/tani + dw_dot_dmu;

    double wpb_dot = B_dot * pb_sec * pb_sec * 0.5 / M_PI;
  cerr << "7" << endl;

    cout << 
      "A far-fetched idea:\n"
      "Expected proper-motion derivative due to radial velocity:\n"
      "\n"
      " r-dot = " << dist_dot << " km/s\n"
      " -> mu-dot = " << mu_dot << " mas/yr^2\n"
      " -> pb-dot contribution = " << wpb_dot
       << endl;
  }

  double x_km = x*c;
  double x_ext = x_km/dist / mas;

  cout << endl <<
    "*******************************************************************\n"
    "Extent of orbit on sky= " << x_ext << " mas\n"
       << endl;

  cout << 
    "*******************************************************************\n"
    "Mass function: " << mass_func << " \261 " << mass_func_err <<
    " solar masses\n"
       << endl;

   cerr << "8" << endl;
 if ( (eph.parmStatus[EPH_KIN] > 0 || eph.parmStatus[EPH_SINI] > 0)
       && eph.parmStatus[EPH_M2] > 0 ) {

    double m2     = eph.value_double [EPH_M2];
    double m2_err = eph.error_double [EPH_M2];

    double pulsar_mass, pulsar_mass_err;
    eph.m1 (pulsar_mass, pulsar_mass_err);

    cout << "Using: \n"
      " m2   = " << m2 << " \261 " << m2_err << " solar masses\n"
      " sini = " << sini << " \261 " << sini_err << "\n\n"
      
      "-> pulsar mass = " << pulsar_mass << " \261 " << pulsar_mass_err 
	 << " solar masses\n";

    double Pb_dot_GR=0;
    eph.GR_Pb_dot (Pb_dot_GR);

    double w_dot_GR=0;
    eph.GR_omega_dot (w_dot_GR);

    double x_dot_GR=0;
    eph.GR_x_dot (x_dot_GR);

    double Omega_p=0;
    eph.GR_Omega_p (Omega_p);

    cout << "-> Pb_dot (GR) = " << Pb_dot_GR << endl;
    cout << "-> w_dot  (GR) = " << w_dot_GR << endl;
    cout << "-> x_dot  (GR) = " << x_dot_GR << endl;
    cout << "-> spin-orbit precession = " << Omega_p << " deg/yr" << endl;

    double m2exp = 0.0;
    eph.m2 (m2exp, 1.4);

    cout << "\nAssume a 1.4 solar mass pulsar:\n"
      "-> companion mass = " << m2exp << endl;

    double mt, mt_err;
    if (eph.GR_omega_dot_mtot (mt, mt_err) == 0) {
      cout << "\nomega_dot GR-> total mass = " << mt << " \261 " << mt_err 
	   << " solar masses\n" << endl;
      double m2, m2_err;
      if (eph.GR_omega_dot_m2 (m2, m2_err) == 0) {
	cout << "omega_dot GR-> m2 = " << m2 << " \261 " << m2_err 
	     << " solar masses\n" << endl;
      }
    }
  }

  return 0;
}
