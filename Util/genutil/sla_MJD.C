#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "MJD.h"
#include <math.h>

#define F77_sla_gmst F77_SLA(sla_gmst,SLA_GMST)
extern "C" double F77_sla_gmst(double *);

double MJD::LST (float longitude) const
{
  double passed_MJD = this->in_days();
  double gmst = F77_sla_gmst(&passed_MJD);
  double lst = gmst/M_PI*180.0/15.0 + longitude/360.0*24.0;
  while (lst<0.0) lst+=24.0;
  while (lst>=24.0) lst-=24.0;
  return lst;
}

double MJD::LST (double longitude) const
{
  double passed_MJD = this->in_days();
  double gmst = F77_sla_gmst(&passed_MJD);
  double lst = gmst/M_PI*180.0/15.0 + longitude/360.0*24.0;
  while (lst<0.0) lst+=24.0;
  while (lst>=24.0) lst-=24.0;
  return lst;
}

