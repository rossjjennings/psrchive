#include "MJD.h"

#include <math.h>
#include <slalib.h>

double MJD::LST (double longitude) const
{
  double passed_MJD = this->in_days();
  double gmst = slaGmst(passed_MJD);
  double lst = gmst/M_PI*180.0/15.0 + longitude/360.0*24.0;
  while (lst<0.0) lst+=24.0;
  while (lst>=24.0) lst-=24.0;
  return lst;
}

