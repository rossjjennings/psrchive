
#include <string.h>
#include <stdio.h>
#ifdef sun
#include <float.h>
#include <sunmath.h>
#endif
#include "MJD.h"
#include "endian.h"
#include "ieee.h"

int ss2hhmmss (int* hours, int* min, int* sec, int seconds)
{
  *hours   = seconds/3600;
  seconds -= 3600 * (*hours);
  *min     = seconds/60;
  seconds -= 60 * (*min);
  *sec     = seconds;

  return 0;
}

char * MJD::printdays() const {
  static char permanent[10];
  sprintf(permanent, "%d",days);
  return (permanent);
}

char * MJD::printhhmmss() const {
  static char permanent[10];
  int hh, mm, ss;

  ss2hhmmss (&hh, &mm, &ss, secs);

  sprintf(permanent,"%2.2d%2.2d%2.2d",hh,mm,ss);
  return (permanent);
}

char * MJD::printfs() const {
  static char permanent[20];
  char temp[20];
  sprintf(temp,"%.15lf",fracsec);
  strcpy(permanent,&temp[1]);   // Chomp off leading 0
  return (permanent);
}

char * MJD::printall() const {
  static char permanent[40];
  sprintf(permanent,"%s:%s%s",printdays(),printhhmmss(),printfs());
  return (permanent);
}

char* MJD::datestr (char* dstr, const char* format) const
{
  cal_t greg;

  if (gregorian (&greg, NULL) < 0)
    return NULL;

  return cal2str (dstr, greg, format);
}

char * MJD::strtempo(){
  static char permanent[40];
  char temp[20];
  sprintf(temp,"%14.13lf",fracday());
  sprintf(&permanent[0],"%5d%s",days,&temp[1]);
  return (permanent);
}

double MJD::in_seconds() const {
  return((double)days*86400.0+(double)secs+fracsec);
}
double MJD::in_days() const {
  return((double)days + ((double)secs+fracsec)/86400.0);
}
double MJD::in_minutes() const {
  return((double) days*1440.0 + ((double)secs+fracsec)/60.0);
}

int MJD::intday() const {
  return(days);
}

double MJD::fracday() const {
    return((double)secs/86400.0 + fracsec/86400.0);
}

MJD& MJD::operator = (const MJD &in_mjd)
{
  if (this != &in_mjd) {
    days = in_mjd.days;
    secs = in_mjd.secs;
    fracsec = in_mjd.fracsec;
  }
  return *this;
}

const MJD operator + (const MJD &m1, const MJD &m2) {
  return MJD(m1.days + m2.days,
	     m1.secs + m2.secs,
	     m1.fracsec + m2.fracsec); // Let constructor do the dirty work.
}

const MJD operator - (const MJD &m1, const MJD &m2) {
  return MJD(m1.days - m2.days,
	     m1.secs - m2.secs,
	     m1.fracsec - m2.fracsec); // Let constructor do the dirty work.
}

const MJD operator / (const MJD &m1, double divisor) {
  double ddays = ((double) m1.days) / divisor;
  double dsecs = ((double) m1.secs) /divisor; 
  double dfracsec = m1.fracsec / divisor;
  return MJD(ddays,dsecs,dfracsec);
}

const MJD operator + (const MJD &m1, double sss) {
  double secs_add = m1.fracsec + sss;
  return MJD((double)m1.days,(double)m1.secs,secs_add);
}

const MJD operator - (const MJD &m1, double sss) {
  double secs_take = m1.fracsec - sss;
  return MJD((double)m1.days,(double)m1.secs,secs_take);
}

int operator > (const MJD &m1, const MJD &m2) {
  if (m1.days != m2.days) return (m1.days>m2.days);
  if (m1.secs != m2.secs) return (m1.secs>m2.secs);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(m1.fracsec-m2.fracsec)<precision_limit) return(0);
  else return (m1.fracsec>m2.fracsec);
}

int operator >= (const MJD &m1, const MJD &m2) {
  if (m1.days != m2.days) return (m1.days>m2.days);
  if (m1.secs != m2.secs) return (m1.secs>m2.secs);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(m1.fracsec-m2.fracsec)<precision_limit) return(1);
  else return (m1.fracsec>m2.fracsec);
}

int operator < (const MJD &m1, const MJD &m2) {
  if (m1.days != m2.days) return (m1.days<m2.days);
  if (m1.secs != m2.secs) return (m1.secs<m2.secs);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(m1.fracsec-m2.fracsec)<precision_limit) return(0);
  else return (m1.fracsec<m2.fracsec);
}

int operator <= (const MJD &m1, const MJD &m2) {
  if (m1.days != m2.days) return (m1.days<m2.days);
  if (m1.secs != m2.secs) return (m1.secs<m2.secs);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(m1.fracsec-m2.fracsec)<precision_limit) return(1);
  else return (m1.fracsec<m2.fracsec);
}

int operator == (const MJD &m1, const MJD &m2){
  double precision_limit = 2*pow(10,-DBL_DIG);
  if ((m1.days == m2.days) &&
      (m1.secs == m2.secs) &&
      (fabs(m1.fracsec-m2.fracsec)<precision_limit)) 
      return (1);
  else
      return (0);  
}

int operator != (const MJD &m1, const MJD &m2){
  double precision_limit = 2*pow(10,-DBL_DIG);
  if ((m1.days != m2.days) ||
      (m1.secs != m2.secs) ||
      (fabs(m1.fracsec-m2.fracsec)>precision_limit)) 
      return (1);
  else
      return (0);  
}

int MJD::print (FILE *stream){
  fprintf(stream, "%s", printall());
  return(0);
}

int MJD::println (FILE *stream) {
  fprintf(stream, "%s\n", printall());
  return(0);
}


#ifdef sun
int parse_float128 (long double mjd, double* ndays, double* seconds, 
		    double* fracseconds)
{
  unsigned long intdays = (unsigned long) mjd;
  long double back_again = (long double) intdays;
  if (back_again > mjd) {
    back_again  -= 1.0;
  }
  *ndays = (double) back_again;

  /* calculate number of seconds left */
  mjd = (mjd - back_again) * 86400;
  unsigned long intseconds = (unsigned long) mjd;
  back_again = (long double) intseconds;
  if (back_again > mjd) {
    back_again  -= 1.0;
  }
  *seconds = (double) back_again;

  /* and fractional seconds left */
  *fracseconds = (double) (mjd - back_again);
  return 0;
}

MJD::MJD(float128 mjd) {
  double ndays = 0.0, seconds = 0.0, fracseconds = 0.0;
  parse_float128 ((long double)mjd, &ndays, &seconds, &fracseconds);
  *this = MJD (ndays,seconds,fracseconds);
}

#else

MJD::MJD(float128 mjd) {
  double ndays = 0.0, fracdays = 0.0, seconds = 0.0, fracseconds = 0.0;

  /* Stuart's ieee.C function */
  cnvrt_long_double ((u_char*) &mjd, &ndays, &fracdays);
  seconds = fracdays * 86400;
  fracseconds = fmod (seconds, 1.0);
  seconds -= fracseconds;
  *this = MJD (ndays,seconds,fracseconds);
}

#endif

MJD::MJD(double dd, double ss, double fs){
  int sec_to_add, days_to_add;
  int positive;

  // Everything should be positive - or everything should
  // be negative. If not modify accordingly.

  if (dd + (ss+fs)/86400.0 > 0.0)
    positive = 1;
  else
    positive = 0;

  if (positive) {
    if (fs < 0.0) {
      sec_to_add = (int) (1.0-1.0 * fs);
      ss -= sec_to_add;
      fs += sec_to_add;
    }

    if (ss< 0.0) {
      days_to_add = (int) (1.0 - ss/86400.0);
      dd -= days_to_add;
      ss += 86400.0 * days_to_add;
    }

    // Now, make dd and ss integers - give remainder
    // to lower order members.
    
    if (fmod (dd,1.0)!=0.0){
      ss += 86400.0 * (fmod(dd,1.0));
      dd = (int) dd;
    }

    if (fmod (ss,1.0)!=0.0) {
      fs += fmod(ss,1.0);
      ss = (int) ss;
    }

    // Make sure fractional seconds are truly fractional
    // round to the nearest fempto second
    fs += 5e-16;
    if (fs>=1.0) {
      sec_to_add = (int) fs;
      fs = fmod(fs,1.0);
      ss += sec_to_add;
    }
    else {
      fs -= 5e-16;
    }
    // Make sure that there aren't too many seconds'

    days_to_add =0;
    if (ss>(86400-1)) {
      days_to_add = (int) (ss/86400);
      ss = (int) fmod((double)ss,86400);
    }
    dd += days_to_add;

    // install the values.

    days = (int) (dd+0.5);
    secs = (int) (ss+0.5);
    fracsec = fs; 
  }

  /* If the whole thing is -ve act differently */

  else {
    if (fs <= -1.0) {
      sec_to_add = (int) (0.0 - fs);
      ss -= sec_to_add;
      fs += sec_to_add;
    }

    if (ss< -86400.0) {
      days_to_add = (int) (0.0-ss/86400.0);
      dd -= days_to_add;
      ss += 86400.0 * days_to_add;
    }

    // Now, make dd and ss integers - give remainder
    // to lower order members.

    if (fmod (-1.0*dd,1.0)!=0.0){
      ss -= 86400.0 * (fmod(-1.0*dd,1.0));
      dd = -1.0 * (int) (fabs(dd));
    }

    if (fmod (-1.0*ss,1.0)!=0.0) {
      fs -= fmod(-1.0*ss,1.0);
      ss = (int) (fabs(ss));
    }

    // Make sure fractional seconds are truly fractional

    sec_to_add =0;
    if (fs<= -1.0) {
      sec_to_add = (int) (-1.0*fs);
      fs = -1.0 * fmod(-1.0*fs,1.0);
    }

    ss -= sec_to_add;

    // Make sure that there are not too many seconds

    days_to_add =0;
    if (ss<(-86399)) {
      days_to_add = (int) (-1*ss/86400);
      ss = -1 * (int) (fmod((double) -1.0*ss,86400));
    }
    dd -= days_to_add;

    // install the values.

    days = -1 * (int) (-1*dd+0.5);
    secs = -1 * (int) (-1*ss+0.5);
    fracsec = fs; 
  }
}

MJD::MJD(int d, int s, double f) {
  *this = MJD((double)d,(double)s,f);
}

// Converts a string containing utc fields yyyy ddd hh mm ss to internal format
MJD::MJD (char* utc_string)
{
  utc_t utc;
  str2utc (&utc, utc_string);

  /*
char temp [40];
fprintf (stderr, "Passing %s to MJD(utc) constructor\n", 
	 utc2str (temp, utc, "yyyy-ddd-hh-mm-ss"));
  */

  *this = MJD(utc);
}

double MJD::LST (float longitude) const
{
  utc_t  utc;
  double lst;

  UTC (&utc, NULL);
  utc_f2LST (&lst, utc, fracsec, longitude);
  return lst;
}

int MJD::UTC (utc_t* utc, double* fsec) const
{
  cal_t  greg;

  gregorian (&greg, fsec);
  return cal2utc (utc, greg);
}

int MJD::gregorian (cal_t* gregdate, double* fsec) const
{
  int julian_day = days + 2400001;

  int n_four = 4  * (julian_day+((6*((4*julian_day-17918)/146097))/4+1)/2-37);
  int n_dten = 10 * (((n_four-237)%1461)/4) + 5;

  gregdate->tm_year  = n_four/1461 - 4712;
  gregdate->tm_month = (n_dten/306+2)%12 + 1;
  gregdate->tm_day   = (n_dten%306)/10 + 1;

  ss2hhmmss (&gregdate->tm_hour, &gregdate->tm_min, &gregdate->tm_sec, secs);

  if (fsec)
    *fsec = fracsec;

  return 0;
}

// construct an MJD from a UTC
MJD::MJD (const utc_t& utc)
{
  cal_t greg;

  utc2cal (&greg, utc);

  /*
char temp [40];
fprintf (stderr, "Passing %s to MJD(cal) constructor\n", 
	 cal2str (temp, greg, "yyyy-MM-dd-hh-mm-ss"));
  */

  Construct (greg);
}

// construct an MJD from a gregorian
MJD::MJD (const cal_t& greg)
{
  Construct (greg);
}

MJD::Construct (const cal_t& greg)
{
  days = (1461*(greg.tm_year-(12-greg.tm_month)/10+4712))/4
    +(306*((greg.tm_month+9)%12)+5)/10
    -(3*((greg.tm_year-(12-greg.tm_month)/10+4900)/100))/4
    +greg.tm_day-2399904;

  // Work out seconds, fracsecs always zero.
  secs = 3600.0 * greg.tm_hour + 60.0 * greg.tm_min + greg.tm_sec;
  fracsec = 0.0;
}

