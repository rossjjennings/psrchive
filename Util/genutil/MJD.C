
#include <string.h>
#include <stdio.h>
#ifdef sun
#include <float.h>
#include <sunmath.h>
#endif
#include <math.h>
#include <iostream>
#include "MJD.h"
#include "machine_endian.h"
#include "ieee.h"

int MJD::verbose = 0;

int ss2hhmmss (int* hours, int* min, int* sec, int seconds)
{
  *hours   = seconds/3600;
  seconds -= 3600 * (*hours);
  *min     = seconds/60;
  seconds -= 60 * (*min);
  *sec     = seconds;

  return 0;
}

// no static kludgeyness, no memory leaks
string MJD::printdays (unsigned precision) const
{
  char* temp = new char [precision + 10];
  sprintf (temp, "%d", days);
  string output = temp;
  if (precision > 0)  {
    sprintf (temp, "%*.*lf", precision+3, precision, fracday());
    char* period = strchr (temp, '.');
    output += period;
  }
  delete [] temp;
  return output;
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
  cerr.precision(15);
  if(verbose) cerr << endl << "MJD::MJD - constructing with values " << dd << "\t" << ss << "\t" << fs << endl;
  // Everything should be positive - or everything should
  // be negative. If not modify accordingly.
  int sign;
  if (dd + (ss+fs)/86400.0 > 0.0) sign = 1;
  else sign = -1;
  int chsecs, chdays;
  if(sign*fs<0.0){
    chsecs = (int)fs - sign;
    ss += chsecs;
    fs -= chsecs;
  }
  if(verbose) cerr << "MJD::MJD - fracsec with sign " << sign << "\t" << dd << "\t" << ss << "\t" << fs << endl;
  if(sign*ss<0.0){
    chdays = (int)(ss/86400.0) - sign;
    dd += chdays;
    ss -= chdays*86400.0;
  }

  if(verbose) cerr << "MJD::MJD - sec with sign " << sign << "\t" << dd << "\t" << ss << "\t" << fs << endl;
  // Now, make dd and ss integers - give remainder
  // to lower order members.
  if (fmod (dd,1.0)!=0.0){
    ss += sign * 86400.0 * (fmod(dd,1.0));
    dd = (int) dd;
  }  
  if (fmod (ss,1.0)!=0.0) {
    fs += sign*fmod(ss,1.0);
    ss = (int) ss;
  }
  
  if(verbose) cerr << "MJD::MJD - integer days and secs " << dd << "\t" << ss << "\t" << fs << endl;
  // Make sure fractional seconds are truly fractional
  // round to the nearest fempto second
  fs += sign*5e-16;
  if (fabs(fs)>1.0) {
    chsecs = (int) fs;
    fs -= sign*chsecs;
    ss += sign*chsecs;
  } else fs -= sign*5e-16;

  if(verbose) cerr << "MJD::MJD - modified fracsecs " << dd << "\t" << ss << "\t" << fs << endl;
  // Make sure that there aren't too many seconds'
  if (fabs(ss)>(86400-1)) {
    chdays = (int) (ss/86400);
    ss = (int) fmod((double)ss,86400);
    dd += sign*chdays;
  }
  
  if(verbose) cerr << "MJD::MJD - modified secs " << dd << "\t" << ss << "\t" << fs << endl;
  // install the values.
  days = (int) (dd+sign*0.5);
  secs = (int) (ss+sign*0.5);
  fracsec = fs; 
  if(verbose) cerr << "MJD::MJD - final values " << days << "\t" << secs << "\t" << fracsec << endl << endl;
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

int
  // long long to get a 64-bit unsigned
MJD::Construct(unsigned long long bat)
{
  // Not sure how to do 64-bit constants so let's construct it
  unsigned long long microsecPerDay;

  microsecPerDay = 86400; microsecPerDay *= 1000000;

  days = bat/microsecPerDay;
  secs = (bat%microsecPerDay)/1000000;
  fracsec = 1.0e-6 * (bat%100000);

  return 0;
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

