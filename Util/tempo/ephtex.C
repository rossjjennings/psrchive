// /////////////////////////////////////////////////////////////////////
//
// Given a pulsar name, plot the annual-orbital parallax term
//
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>

#include "psrephem.h"
#include "ephio.h"

void usage()
{
  cout << "\npsraux: print stuff about the pulsar from various theories"
    "\n"
    "  -v        verbose\n"
       << endl;
}

const double au     = 1.495978e8;   // km
const double c      = 2.99793e5;    // km/s
const double parsec = 3.085678e13;  // km
const double year   = 365.25 * 24 * 3600; // seconds
const double mas    = M_PI/(60.0*60.0*180.0*1000.0);  // radians
const double deg    = 180.0/M_PI;

#define sqr(x) (x*x)

int main (int argc, char ** argv)
{
  bool verbose = false;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "hv")) != -1) {
    switch (gotc) {
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
  cerr << endl;

  // ephio parms of interest
  // EPH_RAJ    - alpha     - right ascension in turns
  // EPH_DECJ   - delta     - declination in turns

  // EPH_PMDEC  - mu_delta  - proper motion in declination (mas/yr)
  // EPH_PMRA   - mu_alpha  - proper motion in right ascension (mas/yr)

  // EPH_PX     -           - parallax in mas
  // EPH_ASCN   - big Omega - longitude of ascending node

  // psrephem has parmStatus and value_double and error_double

  double mu_alpha = eph.value_double [EPH_PMRA];
  double mu_aerr  = eph.error_double [EPH_PMRA];

  double mu_delta = eph.value_double [EPH_PMDEC];
  double mu_derr  = eph.error_double [EPH_PMDEC];

  double delta = eph.value_double [EPH_DECJ] * M_PI;

  // mu_alpha *= sqr(cos(delta));

  // composite proper motion
  double pm = sqrt (sqr(mu_alpha) + sqr(mu_delta));
  double mu_aesq = mu_aerr * mu_aerr;
  double mu_desq = mu_derr * mu_derr;
  double pm_err = sqrt(sqr(mu_alpha)*mu_aesq + sqr(mu_delta)*mu_desq)/pm;
  double pm_f = pm_err / pm;

  double phi = atan2 (mu_alpha, mu_delta);
  double phi_err = sqrt(sqr(mu_alpha)*mu_desq + sqr(mu_delta)*mu_aesq)/(pm*pm);

  double px = eph.value_double [EPH_PX];
  double px_err = eph.error_double [EPH_PX];

  double pb = eph.value_double [EPH_PB];
  double pb_err = eph.error_double [EPH_PB];
  double pb_dot = eph.value_double [EPH_PBDOT] * 1e-12;
  double pb_dot_err = eph.error_double [EPH_PBDOT] * 1e-12;

  // distance to pulsar in km
  double dist = au / (px * mas);
  double dist_err = dist * px_err/px;

  string jname = eph.value_str[EPH_PSRJ];

  
  cout <<
    "\\begin{center}\n"
    "{\\bf Observed and Derived Parameters for PSR J" << jname << "}\\\\\n"
    "\\small\n"
    "\\begin{tabular}{l|lr} \\hline \\hline \n"

    "$\\alpha$ (J2000)     &  \\\\\n"


    "Working from Shklovskii's pm-induced pdot **********************\n\n"
    "Using:\n"
    " pm_ra  = " << mu_alpha << " \261 " << mu_aerr << " mas/yr\n"
    " pm_dec = " << mu_delta << " \261 " << mu_derr << " mas/yr\n"
    " -> pm_tot = " << pm << " \261 " << pm_err  << " mas/yr\n"
    "    pm_phi = " << phi*deg << " \261 " << phi_err  << " deg\n"
    " Pb     = " << pb << " \261 " << pb_err << " days\n"
    " Pb-dot = " << pb_dot << " \261 " << pb_dot_err << "\n" << endl;

  double pb_f = pb_err / pb;
  double pb_dot_f = pb_dot_err / pb_dot;

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
       << endl;

  // kinematic contribution to period derivatives (Shklovskii)
  double Ski = pm_radsec * pm_radsec * dist / c;
  double Ski_err = Ski * sqrt (sqr(2.0*pm_f) + sqr(dist_err));

  double SPb_dot = Ski * (pb * 86400.0);
  double SPb_dot_err = SPb_dot * sqrt (sqr(Ski_err) + sqr(pb_err));

  cout << 
    "Alternatively, use the parallax-derived distance and proper motion\n"
    "to derive limits on the orbital period derivative:\n\n"
    "-> S.Pb-dot = " << SPb_dot << " \261 " << SPb_dot_err << "\n" << endl;

  // cout << "-> Shklovskii = " << Shklovskii << " s-1\n" << endl;

  double rf = eph.value_double [EPH_F];
  double rf_err = eph.error_double [EPH_F];
  double rP = 1.0 / rf;
  double rP_err = rP * rf_err / rf;

  double rf_dot = eph.value_double [EPH_F1];
  double rf_dot_err = eph.error_double [EPH_F1];
  double rP_dot = - rf_dot / (rf*rf);
  double rP_dot_err = rP_dot * sqrt(sqr(2*rf_err/rf) + sqr(rf_dot_err/rf_dot));

  double Ski_P_dot = Ski * rP;

  double residual_P_dot = rP_dot - Ski_P_dot;

  double char_age = 0.5 * rP / residual_P_dot;

  cout << "Also, rotational P on P-dot:\n"
    " f       = " << rf     << " \261 " << rf_err << " s-1\n"
    " fdot    = " << rf_dot << " \261 " << rf_dot_err << " s-2\n"
    " -> P    = " << rP     << " \261 " << rP_err  << " s\n"
    "    Pdot = " << rP_dot << " \261 " << rP_dot_err  << "\n"
    "\n"
    "Subtract Pdot due to proper motion at parallax distance:\n"
    "  S.P-dot = " << Ski_P_dot << "\n"
    "\n"
    "-> characteristic age = " << char_age / (86400.0 * 365.25) << endl;


  cout << endl <<
    "Working from Kopeikin, 1996, ApJ 467:L93-L95 **********************\n"
       << endl;

  // From Kopeikin, 1996, ApJ, 467, L93.  Equation 11
  // constraint on i from xdot, greatest when ddx_dOmega = 0, or
  double Omega = atan2 (-mu_alpha, mu_delta);
  double sOmega = sin(Omega);
  double cOmega = cos(Omega);

  double x     = eph.value_double [EPH_A1];
  double x_err = eph.error_double [EPH_A1];

  double xdot     = eph.value_double [EPH_XDOT] * 1e-12;
  double xdot_err = eph.error_double [EPH_XDOT] * 1e-12;

  double sini     = eph.value_double [EPH_SINI];
  double sini_err = eph.error_double [EPH_SINI];

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

  if (eph.parmStatus[EPH_ASCN] > 0) {

    // considering the prefered Omega from the par file
    Omega = eph.value_double [EPH_ASCN];
    double Omega_err = eph.error_double [EPH_ASCN];

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
			     sqr(dtani_dmu_alpha) * mu_aesq +
			     sqr(dtani_dmu_delta) * mu_desq );

    double imid = atan(tani);
    double imind = imid - atan(tani-tani_err);
    double imaxd = atan(tani+tani_err) - imid;
    double i_err = max (imind, imaxd);

    double sini = sin(imid);
    double sinimind = sini - sin(imid - imind);
    double sinimaxd = sin(imid + imaxd) - sini;
    double sini_err = max (sinimind, sinimaxd);

    cout << 
      "-> tan(i) = " << tani << " \261 " << tani_err << "\n"
      "   sin(i) = " << sini << " \261 " << sini_err << "\n"
      "   i      = " << imid*deg << " \261 " << i_err*deg << "\n" << endl;

    double rad_day1 = (mu_alpha*cOmega + mu_delta*sOmega) * mas/365.25;
    double rad_day2 = (-mu_alpha*sOmega + mu_delta*cOmega) * mas/365.25;

    double kopeikin14 = pb*pb * rad_day1 * rad_day2 / (2.0*M_PI * sini * tani);

    cout << 
      "Using the time derivative of Eqn. 14 and 10 with  above values:\n"
      "-> mu-induced i-dot contribution to Pb-dot = "
	 << kopeikin14 << "\n" << endl;

  }

  return 0;
}
