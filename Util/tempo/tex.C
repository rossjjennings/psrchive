// //////////////////////////////////////////////////////////////////////////
//
// tex.C - code to output LaTeX formatted strings from a psrephem class
//
// code largely stolen from Russell's thesis work
// modified by Willem van Straten
//
// //////////////////////////////////////////////////////////////////////////

#include <vector>
#include <math.h>
#include <iostream>

#include "psrephem.h"
#include "ephio.h"

extern "C" {
  // routines from the psrinfo library used here
  char* crad2dmse (double pos, double err, int ra, char * name);
  char* eoutchop2 (double val, double err, char * fmt, char * outstr );
}

#define sqr(x) ((x)*(x))

// receives a right ascension, as output by psrinfo
// outputs a LaTeX formatted string
string tex_ra (char* str)
{
  char* s;
  string ret;
  
  s = strtok(str, " :");
  ret += string(s) + "$^{\\mathrm h}$";
  
  s = strtok(NULL, " :");
  ret += string(s) + "$^{\\mathrm m}$";
  
  s = strtok(NULL, ".");
  ret += string(s) + "\\fs";
  
  s = strtok(NULL, " ");
  ret += string(s);

  s = strtok(NULL, " \n");
  ret += "(" + string(s) + ")";
  
  return ret;
}

// receives a declination, as output by psrinfo
// outputs a LaTeX formatted string
string tex_dec (char* str)
{
  char* s;
  string ret;
  
  s = strtok(str, " :");
  ret += string(s) + "\\degr";

  s = strtok(NULL, " :");
  ret += string(s) + "\\arcmin";

  s = strtok(NULL, ".");
  ret += string(s) + "\\farcs";

  s = strtok(NULL, " ");
  ret += string(s);

  s = strtok(NULL, " \n");
  ret += "(" + string(s) + ")";

  return ret;
}

// receives any other value, as output by psrinfo
// outputs a LaTeX formatted string
static char buf [128];
string tex_double (double val, double err)
{
  if (val == 0.0)
    return string ("\\nodata");

  else if (err <=0.0) {
    sprintf (buf, "%.6f", val);
    return string (buf);
  }

  string ret;
  char str[128];
  
  eoutchop2 (val, err, "%30f", str);
  
  char* s = strtok(str, " ");
  char* e = strchr(s, 'E');
  
  if (e!=NULL)
    *e = '\0';

  ret = s;
  s = strtok(NULL, " \n");
  ret += "(" + string(s) + ")";
    
  if (e!=NULL)
    ret += "$\\times 10^{" + string(e+1) + "}$";
  
  return ret;
}

// replaces a character with a string
string c2str (const char* instr, char c, const char* repstr)
{
  string ret;

  int len = strlen (instr);
  int pos;
  for (pos=0; pos<len; pos++) {
    if (instr[pos] == c)
      break;
    ret += instr[pos];
  }
  ret += repstr;

  for (pos++; pos<len; pos++)
    ret += instr[pos];

  return ret;
}

string psrephem::tex_name () const
{
  string ret;

  if (!tempo11)
    return ret;

  if (parmStatus[EPH_PSRJ]==1)
    ret = "PSR J" + value_str[EPH_PSRJ];

  else if (parmStatus[EPH_PSRB]==1)
    ret = "PSR B" + value_str[EPH_PSRB];

  else
    return ret;

  return c2str (ret.c_str(), '-', "$-$");
}

// outputs a LaTex formatted string for the value
// indexed by 'ephi' (as in ephio.h)
string psrephem::tex_val (int ephi, double fac, unsigned precision) const
{
  if (verbose)
    cerr << "psrephem::tex_val(" << ephi << ") entered" << endl;

  if (parmStatus[ephi] < 1)
    return string();

  switch ( parmTypes[ephi] ) {

  case 0:
    if (verbose)
      cerr << "psrephem::tex_val string" << endl;
    return value_str[ephi];

  case 1: 
    if (verbose)
      cerr << "psrephem::tex_val double" << endl;
    return tex_double (value_double[ephi]*fac, error_double[ephi]*fac);

  case 2: { // RAs
    if (verbose)
      cerr << "psrephem::tex_val RA" << endl;
    crad2dmse (value_double[ephi] * 2.0*M_PI,
	       error_double[ephi] * 2.0*M_PI/(24.0*60.0*60.0), 1, buf);
    if (verbose)
      cerr << "psrephem::tex_val RA:" << buf << endl;
    return tex_ra (buf);
  }
  case 3: { // DECs
    if (verbose)
      cerr << "psrephem::tex_val DEC" << endl;
    crad2dmse (value_double[ephi] * 2.0*M_PI,
	       error_double[ephi] * 2.0*M_PI/(360.0*60.0*60.0), 0, buf);
    if (verbose)
      cerr << "psrephem::tex_val DEC:" << buf << endl;
    return tex_dec (buf);
  }
  case 4: { // MJDs
    if (verbose)
      cerr << "psrephem::tex_val MJD" << endl;
    string frac = "  ";
    if (value_double[ephi])
      frac = tex_double (value_double[ephi], error_double[ephi]);
    sprintf (buf, "%i%s", value_integer[ephi], frac.c_str()+1);
    return string (buf);
  }
  case 5:  // integers
    if (verbose)
      cerr << "psrephem::tex_val integer" << endl;
    sprintf (buf, "%i", value_integer[ephi]);
    return string (buf);
  }

  return string();
}

// outputs a LaTex formatted string suitable for labelling the value
// indexed by 'ephi' (as in ephio.h)
const char* psrephem::tex_descriptor (int ephind)
{
  switch (ephind) {
  case EPH_PEPOCH: return "$P$ epoch (MJD)";
  case EPH_RAJ:    return "Right ascension, $\\alpha$ (J2000.0)";
  case EPH_DECJ:   return "Declination, $\\delta$ (J2000.0)";
  case EPH_PMRA:   return "Proper motion, $\\mu_\\alpha$ (mas yr$^{-1}$)";
  case EPH_PMDEC:  return "Proper motion, $\\mu_\\delta$ (mas yr$^{-1}$)";
  case EPH_PB:     return "Orbital period, $P_{\\rm b}$ (days)";
  case EPH_A1:     return "Projected semi-major axis, $x$ (lt-s)";
  case EPH_T0:     return "Epoch of periastron, $T_{0}$ (MJD)";
  case EPH_OM:     return "Longitude of periastron, $\\omega$ (\\degr)";
  case EPH_E:      return "Orbital eccentricity, $e$";
  case EPH_KIN:    return "Orbital inclination, $i$ (\\degr)";
  case EPH_KOM:    return "Longitude of ascension, $\\Omega$ (\\degr)";
  case EPH_PX:     return "Annual parallax, $\\pi$ (mas)";
  case EPH_PBDOT:  return "$\\dot P_{\\rm b} (10^{-12})$"; 
  case EPH_OMDOT:  return "$\\dot \\omega$ (\\degr yr$^{-1}$)";
  case EPH_M2:     return "Companion mass, m$_2$ (M$_{\\odot})$";
  default:
    return NULL;
  }
}

// outputs a LaTex formatted string with table entries for
// valid entries in the psrephem class.
// feel free to add more as you see fit
string psrephem::tex () const
{
  bool binary = parmStatus[EPH_BINARY] > 0;

  string bw = " \t \\dotfill & ";
  string nl = " \\\\\n";

  string retval =
      + tex_descriptor(EPH_RAJ) +  bw + tex_val (EPH_RAJ) + nl
      + tex_descriptor(EPH_DECJ) + bw + tex_val (EPH_DECJ) + nl;

  double mu, mu_err;
  pm (mu, mu_err);
  retval += "Composite proper motion, $\\mu$ (mas yr$^{-1}$)" + bw 
       + tex_double(mu, mu_err) + nl;

  double ph, ph_err;
  phi (ph, ph_err);
  retval += "Celestial position angle, $\\phi_\\mu (\\degr)$" + bw
       + tex_double(ph*180.0/M_PI, ph_err*180.0/M_PI) + nl;

  retval += tex_descriptor(EPH_PX) + bw + tex_val (EPH_PX) + nl;

  double p, p_err;
  P (p, p_err);
  retval += "Pulse period, $P$ (ms)" + bw
       + tex_double (p*1e3, p_err*1e3) + nl;

  retval += tex_descriptor(EPH_PEPOCH) + bw + tex_val (EPH_PEPOCH) + nl;

  double p_dot, p_dot_err;
  P_dot (p_dot, p_dot_err);

  retval += "Period derivative, $\\dot{P}$ (10$^{-20}$)" + bw
       + tex_double (p_dot*1e20, p_dot_err*1e20) + nl;

  if (binary)
  {
    retval += tex_descriptor(EPH_PB)  + bw + tex_val (EPH_PB) + nl
	 + tex_descriptor(EPH_A1)  + bw + tex_val (EPH_A1) + nl
	 + tex_descriptor(EPH_E)   + bw + tex_val (EPH_E) + nl
	 + tex_descriptor(EPH_T0)  + bw + tex_val (EPH_T0) + nl
	 + tex_descriptor(EPH_OM)  + bw + tex_val (EPH_OM) + nl
	 + tex_descriptor(EPH_KOM) + bw + tex_val (EPH_KOM) + nl
	 + tex_descriptor(EPH_KIN) + bw + tex_val (EPH_KIN) + nl
	 + tex_descriptor(EPH_M2)  + bw + tex_val (EPH_M2) + nl
	 + tex_descriptor(EPH_PBDOT) + bw + tex_val (EPH_PBDOT) + nl
	 + tex_descriptor(EPH_OMDOT) + bw + tex_val (EPH_OMDOT) + nl
      ;
  }

  double mp, mp_err;
  m1 (mp, mp_err);
  retval += "Pulsar mass, m$_{\\rm p}$ (M$_{\\odot})$" + bw
    + tex_double (mp, mp_err) + nl;

  double beta, beta_err;
  quadratic_Doppler (beta, beta_err);
  retval += "Quadratic Doppler shift, $\\beta$ (10$^{-20}$s$^{-1}$)" + bw
    + tex_double (beta*1e20, beta_err*1e20) + nl;

  double p_dot_int = p_dot - p * beta;
  double p_dot_int_err = p_dot_err 
    + p * beta * sqrt(sqr(p_err/p)+sqr(beta_err/beta));

  retval += "Intrinsic period derivative, $\\dot P_{\\rm int}$ (10$^{-20}$)"
    + bw + tex_double (p_dot_int*1e20, p_dot_int_err*1e20) + nl;

  double char_age = 0.5 * p / p_dot_int / (86400.0e9 * 365.25);
  double char_age_err = char_age
    * sqrt(sqr(p_err/p)+sqr(p_dot_int_err/p_dot_int));

  retval += "Characteristic age, $\\tau_c$ (Gyr)"
    + bw + tex_double (char_age, char_age_err) + nl;

  double pb = value_double [EPH_PB];
  double pb_err = error_double [EPH_PB];

  double SPb_dot = beta * (pb * 86400.0);
  double SPb_dot_err = SPb_dot * sqrt (sqr(beta_err/beta) + sqr(pb_err/pb));

  retval += "Induced $\\dot P_{\\rm b} (10^{-12})$"
    + bw + tex_double (SPb_dot*1e12, SPb_dot_err*1e12) + nl;

  double GRPb_dot;
  if (GR_Pb_dot (GRPb_dot) < 0)
    cerr << "******* Error GR_Pb_dot!" << endl;
  GRPb_dot *= 1e12;
  retval += "$\\dot P_{\\rm b}^{\\rm GR} (10^{-12})$" + bw
    + tex_double (GRPb_dot, 0) + nl;

  double GRx_dot;
  if (GR_x_dot (GRx_dot) < 0)
    cerr << "******* Error GR_x_dot!" << endl;
  GRx_dot *= 1e20;
  retval += "$\\dot x^{\\rm GR} (10^{-20})$" + bw
    + tex_double (GRx_dot, 0) + nl;

  double GRw_dot;
  if (GR_omega_dot (GRw_dot) < 0)
    cerr << "******* Error GR_w_dot!" << endl;
  // GRw_dot *= 1e12 * 180/M_PI;
  retval += "$\\dot \\omega^{\\rm GR} (\\degr yr^{-1})$" + bw
    + tex_double (GRw_dot, 0) + nl;

  double gamma;
  if (GR_gamma (gamma) < 0)
    cerr << "******* Error GR_w_dot!" << endl;
  cout << "GAMMA: " << gamma << endl;

#if 0
  printf("\\\\\n");
  printf("Surface magnetic field, $B_{\\rm surf}$ (10$^8$ Gauss)\\dotfill\n");
  for (i=0 ; i < psrs.size(); i++)
  {
    if (psrs[i]->pbdot.value <= 0.0)
	printf("    & \\nodata\n");
      else
    printf("    & %.2f\n", 1.0e-8*
	   sqrt(psrs[i]->pb.value * psrs[i]->pbdot.value*1e-15)*3.2e19);
  }
  printf("\\\\\n");
#endif

  return retval;
}
