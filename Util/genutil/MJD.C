
#include <string.h>
#include <stdio.h>
#ifdef sun
#include <float.h>
#include <sunmath.h>
#endif
#include <math.h>

#include "MJD.h"
#include "machine_endian.h"
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

char* MJD::datestr (char* dstr, int len, const char* format) const
{
  struct tm greg;

  if (gregorian (&greg, NULL) < 0)
    return NULL;

  strftime (dstr, len, format, &greg);
  return dstr;
}

char * MJD::strtempo() const{
  static char permanent[40];
  char temp[20];
  sprintf(temp,"%14.11lf",fracday());
  char* period = strchr (temp, '.');
  sprintf(&permanent[0],"%5d.%s",days,period+1);
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

MJD& MJD::operator += (const MJD &in_mjd)
{
  *this = *this + in_mjd;
  return *this;
}

MJD& MJD::operator -= (const MJD &in_mjd)
{
  *this = *this - in_mjd;
  return *this;
}

MJD& MJD::operator += (const double & d)
{
  *this = *this + d;
  return *this;
}

MJD& MJD::operator -= (const double & d)
{
  *this = *this - d;
  return *this;
}

MJD& MJD::operator *= (const double & d)
{
  *this = *this*d;
  return *this;
}

MJD& MJD::operator /= (const double & d)
{
  *this = *this/d;
  return *this;
}

const MJD operator + (const MJD &m1, const MJD &m2) {
  return MJD(m1.intday() + m2.intday(),
	     m1.get_secs() + m2.get_secs(),
	     m1.get_fracsec() + m2.get_fracsec()); // Let constructor do the dirty work.
}

const MJD operator - (const MJD &m1, const MJD &m2) {
  return MJD(m1.intday() - m2.intday(),
	     m1.get_secs() - m2.get_secs(),
	     m1.get_fracsec() - m2.get_fracsec()); // Let constructor do the dirty work.
}

const MJD operator + (const MJD &m1, double sss) {
  double secs_add = m1.get_fracsec() + sss;
  return MJD((double)m1.intday(),(double)m1.get_secs(),secs_add);
}

const MJD operator - (const MJD &m1, double sss) {
  double secs_take = m1.fracsec - sss;
  return MJD((double)m1.intday(),(double)m1.get_secs(),secs_take);
}

const MJD operator * (const MJD &m1, double d) {
  double ddays = ((double) m1.intday()) * d;
  double dsecs = ((double) m1.get_secs()) * d; 
  double dfracsec = m1.get_fracsec() * d;
  return MJD(ddays,dsecs,dfracsec);
}

const MJD operator / (const MJD &m1, double divisor) {
  double ddays = ((double) m1.intday()) / divisor;
  double dsecs = ((double) m1.get_secs()) /divisor; 
  double dfracsec = m1.get_fracsec() / divisor;
  return MJD(ddays,dsecs,dfracsec);
}

MJD abs(const MJD & in_mjd) {
  if(in_mjd.intday()>0) return(in_mjd);
  else return(MJD(in_mjd.intday()*-1.0,in_mjd.fracday()*-1.0));
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

MJD::MJD(int intday, double fracday){
  
  int isecs = (int)fracday*86400;
  double fracsecs = fracday*86400 - double(isecs);
  
  *this = MJD(intday, isecs, fracsecs);
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
  struct tm  greg;

  gregorian (&greg, fsec);
  return tm2utc (utc, greg);
}

int MJD::gregorian (struct tm* gregdate, double* fsec) const
{
  int julian_day = days + 2400001;

  int n_four = 4  * (julian_day+((6*((4*julian_day-17918)/146097))/4+1)/2-37);
  int n_dten = 10 * (((n_four-237)%1461)/4) + 5;

  gregdate->tm_year = n_four/1461 - 4712 - 1900; // extra -1900 for C struct tm
  gregdate->tm_mon  = (n_dten/306+2)%12;         // struct tm mon 0->11
  gregdate->tm_mday = (n_dten%306)/10 + 1;

  ss2hhmmss (&gregdate->tm_hour, &gregdate->tm_min, &gregdate->tm_sec, secs);

  if (fsec)
    *fsec = fracsec;

  return 0;
}

// construct an MJD from a UTC
MJD::MJD (const utc_t& utc)
{
  if (Construct (utc) < 0)
    throw ("MJD::MJD(utc_t) construct error");
}

int MJD::Construct (const utc_t& utc)
{
  struct tm greg;
  if (utc2tm (&greg, utc) < 0)  {
    fprintf (stderr, "MJD::Construct(utc_t) error converting to gregorian\n");
    return -1;
  }

#if 0
  char buffer [100];
  strftime (buffer, 50, "%Y-%j-%H:%M:%S", &greg);
  fprintf (stderr, "MJD::Construct utc:%s = greg:%s\n",
	   utc2str (buffer+50, utc, "yyyy-ddd-hh:mm:ss"),
	   buffer);
#endif

  return Construct (greg);
}

MJD::MJD (time_t time)
{
  if (Construct (time) < 0)
    throw ("MJD::MJD(time_t) construct error");
}

int MJD::Construct (time_t time)
{
  struct tm date = *gmtime(&time);
  return Construct (date);
}

// construct an MJD from a gregorian
MJD::MJD (const struct tm& greg)
{
  if (Construct (greg) < 0)
    throw ("MJD::MJD(struct tm) construct error");
}

int MJD::Construct (const struct tm& greg)
{
  int year = greg.tm_year + 1900;
  int month = greg.tm_mon + 1;

  days = (1461*(year-(12-month)/10+4712))/4
    +(306*((month+9)%12)+5)/10
    -(3*((year-(12-month)/10+4900)/100))/4
    +greg.tm_mday-2399904;

  // Work out seconds, fracsecs always zero.
  secs = 3600 * greg.tm_hour + 60 * greg.tm_min + greg.tm_sec;
  fracsec = 0.0;
  return 0;
}

// parses a string of the form 51298.45034 ish
int MJD::Construct (const char* mjdstr)
{
  if (sscanf (mjdstr, "%d", &days) < 1) {
    fprintf (stderr, "MJD::Construct Could not parse '%s'\n", mjdstr);
    return -1;
  }
  char* fracstr = strchr (mjdstr, '.');
  if (fracstr) {
    double fracday;
    if (sscanf (fracstr, "%lf", &fracday) < 1)
      return -1;
    fracday *= 86400.0;
    secs = int (fracday);
    fracsec = fracday - double (secs);
  }
  else {
    fracsec = 0.0;
    secs = 0;
  }
  return 0;
}

