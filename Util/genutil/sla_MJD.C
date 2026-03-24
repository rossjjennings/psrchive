/***************************************************************************
 *
 *   Copyright (C) 2004-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MJD.h"

// #define _DEBUG 1
#include "debug.h"

#include <math.h>
#include <slalib.h>

double MJD::GMST () const
{
  double passed_MJD = this->in_days();

  DEBUG("MJD::GMST mjd=" << passed_MJD);
  double gmst = slaGmst(passed_MJD);
  //double gmst = c_gmst(passed_MJD);
  DEBUG("MJD::GMST gmst=" << gmst);
  return gmst;
}
  
double MJD::LST (double longitude_deg) const
{
  double gmst_rad = this->GMST();
  DEBUG("MJD::LST gmst=" << gmst_rad << " rad");
  double lst_hours = gmst_rad * 12.0/M_PI + longitude_deg * 12.0/180.0;
  DEBUG("MJD::LST lst=" << lst_hours << " hr");

  while (lst_hours<0.0) lst_hours+=24.0;
  while (lst_hours>=24.0) lst_hours-=24.0;
  return lst_hours;
}
