/* $Log: MJD.C,v $
/* Revision 1.1  1998/08/03 06:45:23  mbritton
/* Initial revision
/*
 * Revision 1.7  1996/12/03 05:37:43  mtoscano
 * *** empty log message ***
 *
 * Revision 1.6  1996/09/13 23:13:18  mbailes
 * Added more functions
 *
 * Revision 1.5  1996/09/09 23:12:17  mbailes
 * Added more functions, MJDs can be -ve now.
 * */
#include <string.h>
#include <stdio.h>
#include "MJD.h"

char * MJD::printdays(){
  static char permanent[10];
  sprintf(permanent, "%d",days);
  return (permanent);
}

char * MJD::printhhmmss(){
  static char permanent[10];
  int d = secs;
  int hh = d/3600;
  int mm = (d-3600*hh)/60;
  int ss = (d-3600*hh-60*mm);
  sprintf(permanent,"%2.2d%2.2d%2.2d",hh,mm,ss);
  return (permanent);
}

char * MJD::printfs(){
  static char permanent[20];
  char temp[20];
  sprintf(temp,"%14.12lf",fracsec);
  strcpy(permanent,&temp[1]);   // Chomp off leading 0
  return (permanent);
}

char * MJD::printall(){
  static char permanent[40];
  sprintf(permanent,"%s:%s%s",printdays(),printhhmmss(),printfs());
  return (permanent);
}

char * MJD::strtempo(){
  static char permanent[40];
  char temp[20];
  sprintf(temp,"%14.13lf",fracday());
  sprintf(&permanent[0],"%5d%s",days,&temp[1]);
  return (permanent);
}

double MJD::in_seconds(){
  return((double)days*86400.0+secs+fracsec);
}
double MJD::in_days(){
  return((double)days + (secs+fracsec)/86400.0);
}
double MJD::in_minutes(){
  return((double) days*1440.0 + (secs+fracsec)/60.0);
}

int MJD::intday(){
  return(days);
}

double MJD::fracday(){
    return((double)secs/86400.0 + fracsec/86400.0);
}

MJD operator + (const MJD &m1, const MJD &m2) {
  MJD m3 = m1;
  m3.days += m2.days;
  m3.secs += m2.secs;
  m3.fracsec += m2.fracsec; 
  return MJD(m3.days,m3.secs,m3.fracsec);
}

MJD operator - (const MJD &m1, const MJD &m2) {
  MJD m3 = m1;
  m3.days -= m2.days;
  m3.secs -= m2.secs;
  m3.fracsec -= m2.fracsec; 
  return MJD(m3.days,m3.secs,m3.fracsec); // Let constructor do the dirty work.
}

int operator > (const MJD &m1, const MJD &m2) {
  if (m1.days != m2.days) return (m1.days>m2.days);
  if (m1.secs != m2.secs) return (m1.secs>m2.secs);
  return (m1.fracsec>m2.fracsec);
}

int operator < (const MJD &m1, const MJD &m2) {
  if (m1.days != m2.days) return (m1.days<m2.days);
  if (m1.secs != m2.secs) return (m1.secs<m2.secs);
  return (m1.fracsec<m2.fracsec);
}

int operator == (const MJD &m1, const MJD &m2){
  if ((m1.days == m2.days) &&
      (m1.secs == m2.secs) &&
      (m1.fracsec == m2.fracsec)) 
      return (1);
  else
      return (0);  
}

MJD operator / (const MJD &m1, double divisor) {
  MJD m3 = m1;

  double ddays = ((double) m1.days) / divisor;
  double dsecs = ((double) m1.secs) /divisor; 
  double dfracsec = m1.fracsec / divisor;
  return MJD(ddays,dsecs,dfracsec);
}

MJD operator + (const MJD &m1, double sss) {
  double secs_add = m1.fracsec + sss;
  return MJD((double)m1.days,(double)m1.secs,secs_add);
}

MJD operator - (const MJD &m1, double sss) {
  double secs_take = m1.fracsec - sss;
  return MJD((double)m1.days,(double)m1.secs,secs_take);
}

int MJD::print (FILE *stream){
  fprintf(stream, "%s", printall());
  return(0);
}

int MJD::println (FILE *stream) {
  fprintf(stream, "%s\n", printall());
  return(0);
}

MJD::MJD(double dd, double ss, double fs){
  int sec_to_add, days_to_add;
  int positive;

// Everything should be positive - or everything should
// be negative. If not modify accordingly.

if (dd + (ss+fs)/86400.0 > 0.0) positive = 1;
if (dd + (ss+fs)/86400.0 < 0.0) positive = 0;

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

  sec_to_add =0;
  if (fs>=1.0) {
    sec_to_add = (int) fs;
    fs = fmod(fs,1.0);
  }

  ss += sec_to_add;

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

if (!positive) {
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

MJD::MJD(int d, int s, double f){
  *this = MJD((double)d,(double)s,f);
}

// Converts a string yyyydddhhmmss to internal format

MJD::MJD(char * yyyydddhhmmss){
  int leapyr;
  int month[13];

  int year,ddd,hour,minute,second;

  if (sscanf(yyyydddhhmmss,"%4d%3d%2d%2d%2d",
	     &year,&ddd,&hour,&minute,&second)!=5) {
    fprintf(stderr,"MJD::MJD(char*) Error converting %s to MJD\n",
	    yyyydddhhmmss);
    year = ddd = hour = minute = second = 0;
  }

  if(year%4==0){leapyr=1;}else{leapyr=0;}  
  if(year%100==0 && year%400!=0){leapyr=0;}

  month[0]=0;
  month[1]=31;
  month[2]=28+leapyr;
  month[3]=31;
  month[4]=30;
  month[5]=31;
  month[6]=30;
  month[7]=31;
  month[8]=31;
  month[9]=30;
  month[10]=31;
  month[11]=30;
  month[12]=31;

// Work out integer days.

  for(int i=0;i<13 && ddd>0;i++){

     ddd -= month[i];

           if(ddd<=0){
	   
	   int Month = i;
	   days = ( (1461*(year-(12-Month)/10+4712))/4
                       +(306*((Month+9)%12)+5)/10
                       -(3*((year-(12-Month)/10+4900)/100))/4
                       +ddd+month[i]-2399904  );
	   }
  }

  // Work out seconds, fracsecs always zero.

  secs = 3600.0 * hour + 60.0 * minute + second;
  fracsec = 0.0;
}
