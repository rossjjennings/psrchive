#include <vector>
#include <math.h>

#include "psrephem.h"
#include "ephio.h"

extern "C" {
  char * crad2dms  ( double pos, int ra, char * name );
  char * crad2dmse ( double pos, double err, int ra, char * name );
  char * eoutchop2 ( double val, double err, char * fmt, char * outstr );
}

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

string tex_deg (const char* valstr)
{
  string ret;

  int len = strlen (valstr);
  int pos;
  for (pos=0; pos<len; pos++) {
    if (valstr[pos] == '.')
      break;
    ret = ret + valstr[pos];
  }
  ret += "\\degr";

  for (; pos<len; pos++)
    ret = ret + valstr[pos];

  return ret;
}

string psrephem::tex_val (int ephi, double fac, unsigned precision)
{
  if (parmStatus[ephi] < 1)
    return string();

  switch ( parmTypes[ephi] ) {
  case 0:
    return value_str[ephi];
  case 1: 
    return tex_double (value_double[ephi]*fac, error_double[ephi]*fac);
  case 2:  // RAs
    crad2dmse (value_double[ephi] * 2.0*M_PI,
	       error_double[ephi] * 2.0*M_PI, 1, buf);
    return tex_ra (buf);
  case 3:  // DECs
    crad2dmse (value_double[ephi] * 2.0*M_PI,
	       error_double[ephi] * 2.0*M_PI, 0, buf);
    return tex_ra (buf);
  case 4: { // MJDs
    MJD mjd (value_integer[ephi], value_double[ephi]);
    return mjd.printdays (precision);
  }
  case 5:  // integers
    sprintf (buf, "%i", value_integer[ephi]);
    return string (buf);
  }

  return string();
}

const char* psrephem::tex_descriptor (int ephind)
{
  switch (ephind) {
  case EPH_PEPOCH: return "$P$ epoch (MJD)";
  case EPH_RAJ:   return "Right ascenscion, $\\alpha$ (J2000.0)";
  case EPH_DECJ:  return "Declination, $\\delta$ (J2000.0)";
  case EPH_PMRA:  return "Proper motion, $\\mu_\\alpha$ (mas/yr)";
  case EPH_PMDEC: return "Proper motion, $\\mu_\\delta$ (mas/yr)";
  case EPH_PB:    return "Orbital period, $P_{\\rm b}$ (d)";
  case EPH_A1:    return "Projected semi-major axis, $x=a \\sin i$ (lt-s)";
  case EPH_T0:    return "Epoch of periastron, $T_{0}$ (MJD)";
  case EPH_OM:    return "Longitude of periastron, $\\omega$ (\\degr)";
  case EPH_E:     return "Orbital eccentricity, $e$";
  case EPH_KIN:   return "Orbital inclination, $i$ (\\degr)";
  case EPH_KOM:   return "Longitude of ascension, $\\Omega$ (\\degr)";
  default:        return NULL;
  }
}

// func to actually produce a table for some pulsars
string psrephem::tex ()
{
  bool binary = parmStatus[EPH_BINARY] > 0;

  string bw = "\\dotfill & ";
  string nl = "\\\\\n";

  string retval =
    "\\begin{deluxetable}{lr}\n"
    "\\tablecaption{Astrometric, Spin, Binary and Derived Parameters}\n"
    "\\tablecolumns{%d}\n"
    "\\tablewidth{0pt}\n"
    "\\tablehead{\n"
    "    \\colhead{} \n"
    "      & \\colhead{" + psrname() + "}\n"
    "}\n"
    "\\startdata\n"

      + tex_descriptor(EPH_RAJ) +  bw + tex_val (EPH_RAJ) + nl
      + tex_descriptor(EPH_DECJ) + bw + tex_val (EPH_DECJ) + nl;

  double mu, mu_err;
  pm (mu, mu_err);
  retval += "Composite proper motion, $\\mu$ (mas/yr)" + bw 
       + tex_double(mu, mu_err) + nl;

  double ph, ph_err;
  phi (ph, ph_err);
  retval += "Celestial position angle, $\\phi_\\mu$" + bw
       + tex_double(ph, ph_err) + nl;

  retval += tex_descriptor(EPH_PX) + bw + tex_val (EPH_PX) + nl;

  double p, p_err;
  P (p, p_err);
  retval += "Pulse period, $P$ (ms)" + bw
       + tex_double (p*1e3, p_err*1e3) + nl;

  retval += tex_descriptor(EPH_PEPOCH) + bw + tex_val (EPH_PEPOCH) + nl;

  double p_dot, p_dot_err;
  P_dot (p_dot, p_dot_err);

  retval += "Period derivative, $\\dot{P}$ (10$^{-20}$)" + bw
       + tex_double (p*1e-20, p_err*1e-20) + nl;

  if (binary)
  {
    retval += tex_descriptor(EPH_PB)  + bw + tex_val (EPH_PB) + nl
	 + tex_descriptor(EPH_A1)  + bw + tex_val (EPH_A1) + nl
	 + tex_descriptor(EPH_E)   + bw + tex_val (EPH_E) + nl
	 + tex_descriptor(EPH_T0)  + bw + tex_val (EPH_T0) + nl
	 + tex_descriptor(EPH_OM)  + bw + tex_val (EPH_OM) + nl
	 + tex_descriptor(EPH_KOM) + bw + tex_val (EPH_KOM) + nl
	 + tex_descriptor(EPH_KIN) + bw + tex_val (EPH_KIN) + nl;
  }

#if 0
  printf("Characteristic age, $\\tau_c$ (Gyr)\\dotfill\n");
  for (i=0 ; i < psrs.size(); i++)
  {
    if (psrs[i]->pbdot.value <= 0.0)
	printf("    & \\nodata\n");
      else
	printf("    & %.2f\n", 0.5*365.25*86400.0*1.0e-9*
	   psrs[i]->pb.value/psrs[i]->pbdot.value);
  }
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

  retval += 
    "\\enddata\n"
    "\\end{deluxetable}\n";

  return retval;
}
