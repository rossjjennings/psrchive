#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "MJD.h"
#include <math.h>

#define SLA_gmst SLA_FUNC(sla_gmst,slaGmst)
extern "C" double SLA_gmst(double *);

double MJD::LST (float longitude) const
{
  double passed_MJD = this->in_days();
  double gmst = SLA_gmst(&passed_MJD);
  double lst = gmst/M_PI*180.0/15.0 + longitude/360.0*24.0;
  while (lst<0.0) lst+=24.0;
  while (lst>=24.0) lst-=24.0;
  return lst;
}

double MJD::LST (double longitude) const
{
  double passed_MJD = this->in_days();
  double gmst = SLA_gmst(&passed_MJD);
  double lst = gmst/M_PI*180.0/15.0 + longitude/360.0*24.0;
  while (lst<0.0) lst+=24.0;
  while (lst>=24.0) lst-=24.0;
  return lst;
}

