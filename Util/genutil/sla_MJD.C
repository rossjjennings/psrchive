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
