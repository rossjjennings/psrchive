/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MJD.h"

#include <math.h>
#include <slalib.h>

// #define _DEBUG

#ifdef _DEBUG

#include <iostream>
#include <iomanip>

using namespace std;

#define VERBOSE(x) cerr << x << endl;

double c_gmst (double mjd)
{
  cerr << "c_gmst passed MJD=" << setprecision(15) << mjd << endl;
  
  double D2PI = 6.283185307179586476925286766559e0;
  double S2R = 7.272205216643039903848711535369e-5;

  // Julian centuries from fundamental epoch J2000 to this UT
  double TU=(mjd-51544.5)/36525;

  cerr << "Julian Century=" << TU << endl;
  
  // GMST at this UT
  double sla_GMST = fmod(mjd,1.0)*D2PI+
    (24110.54841e0+
     (8640184.812866e0+
      (0.093104e0 - 6.2e-6*TU)*TU)*TU)*S2R;

  cerr << "GMST before fmod=" << sla_GMST << endl;
  
  return fmod(sla_GMST,D2PI);
}

#else

#define VERBOSE(x)

#endif

double MJD::GMST () const
{
  double passed_MJD = this->in_days();

  VERBOSE("MJD::GMST mjd=" << passed_MJD);
  double gmst = slaGmst(passed_MJD);
  //double gmst = c_gmst(passed_MJD);
  VERBOSE("MJD::GMST gmst=" << gmst);
  return gmst;
}
  
double MJD::LST (double longitude) const
{
  double gmst = this->GMST();
  VERBOSE("MJD::LST gmst=" << gmst);
  double lst = gmst/M_PI*180.0/15.0 + longitude/360.0*24.0;
  VERBOSE("MJD::LST lst=" << lst);

  while (lst<0.0) lst+=24.0;
  while (lst>=24.0) lst-=24.0;
  return lst;
}

#if 0

int MJD::Construct (const struct tm& greg)
{
  int year = greg.tm_year + 1900;
  int month = greg.tm_mon + 1;
  int day = greg.tm_mday;

  int status = 0;
  double mjd = 0.0;

  slaCldj (&year, &month, &day, &mjd, &status);

  secs = 3600 * greg.tm_hour + 60 * greg.tm_min + greg.tm_sec;
  days = 0;
  fracsec = 0.0;

  add_day (mjd);
  settle ();

  return 0;
}

#endif
