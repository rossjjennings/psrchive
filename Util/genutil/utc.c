
/*
$Id: utc.c,v 1.3 1998/08/12 07:10:20 straten Exp $
$Log: utc.c,v $
Revision 1.3  1998/08/12 07:10:20  straten
bug fix - when adding month days to make year day, use previous month
so month -> month-1

Revision 1.2  1998/08/05 12:15:54  straten
added bit to str2utc to cut off extraneous characters before parsing

 * Revision 1.1.1.1  1998/08/03  06:45:23  mbritton
 * start
 *
Revision 1.2  1997/04/10 22:27:00  sasha
New function str2pos()

Revision 1.1  1996/11/18 19:53:55  sasha
Initial revision

Revision 1.1  1996/09/01 17:59:18  willem
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>

#include "utc.h"

/* *************************************************************************
   str2utc - converts a string of characters into a UTC time.

   "str"  contains a string representation of UT time.  This string 
          will be parsed as follows:

	  Instead of typing a long-winded explanation, here are some examples:

	  str == "1997251043045"                ->  1997-251-04-30-45
	  
	  str == "345"                          ->  0000-000-00-03-45
	  
	  str == "1997-51-4-3:45"               ->  1997-051-04-03-45

	  str == "test1997hard251a4is30this45"  ->  1997-251-04-30-45

   "time" will return the UT time represented by "str"
   ************************************************************************* */
int str2utc (utc_t *time, const char* str)
{
  char* temp;
  int   trav;
  int   endstr;
  char  infield;
  int   field_count;
  int   digits;

  time->tm_year = 0;
  time->tm_hour = 0;
  time->tm_yday = 0;
  time->tm_min = 0;
  time->tm_sec = 0;

  temp = strdup (str);

  /* count the number of fields and cut the string off after a year, day,
     hour, minute, and second can be parsed */
  trav = 0; infield = 0;
  field_count = digits = 0;
  while (temp[trav] != '\0') {
    if (isdigit(temp[trav])) {
      digits ++;
      if (!infield) {
	/* count only the transitions from non digits to a field of digits */
	field_count ++;
      }
      infield = 1;
    }
    else {
      infield = 0;
    }
    if (field_count == 5) {
      /* currently in the seconds field */
      temp[trav+2] = '\0';
      break;
    }
    else if (digits == 13) {
      /* enough digits for a date */
      temp[trav+1] = '\0';
      break;
    }
    trav ++;
  }

  endstr = strlen(temp);
  /* cut off any trailing characters that are not ASCII numbers */
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return -1;
  temp [endstr+1] = '\0'; 


  /* parse UTC seconds */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_sec));

  /* cut out seconds and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC minutes */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_min));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC hours */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_hour));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC days */
  trav = endstr;
  while ((trav >= 0) && (endstr-trav < 3) && isdigit(temp[trav]))
    trav--;
  ++trav;  /* when loop ends, trav has gone too far, or points to a nondigit */
  sscanf (temp+trav, "%3d", &(time->tm_yday));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC year */
  trav = endstr;
  while ((trav >= 0) && (endstr-trav < 4) && isdigit(temp[trav]))
    trav--;
  sscanf (temp+trav+1, "%4d", &(time->tm_year));

  free (temp);
  return 0;
} 


int str2cal (cal_t *time, const char* str)
{
  char* temp;
  int   trav;
  int   endstr;
  char  infield;
  int   field_count;
  int   digits;

  time->tm_year = 0;
  time->tm_month = 0;
  time->tm_day = 0;
  time->tm_hour = 0;
  time->tm_min = 0;
  time->tm_sec = 0;

  temp = strdup (str);

  /* count the number of fields and cut the string off after a year, day,
     hour, minute, and second can be parsed */
  trav = 0; infield = 0;
  field_count = digits = 0;
  while (temp[trav] != '\0') {
    if (isdigit(temp[trav])) {
      digits ++;
      if (!infield) {
	/* count only the transitions from non digits to a field of digits */
	field_count ++;
      }
      infield = 1;
    }
    else {
      infield = 0;
    }
    if (field_count == 6) {
      /* currently in the seconds field */
      temp[trav+2] = '\0';
      break;
    }
    else if (digits == 14) {
      /* enough digits for a date */
      temp[trav+1] = '\0';
      break;
    }
    trav ++;
  }

  endstr = strlen(temp);
  /* cut off any trailing characters that are not ASCII numbers */
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return -1;
  temp [endstr+1] = '\0'; 


  /* parse UTC seconds */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_sec));

  /* cut out seconds and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC minutes */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_min));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC hours */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_hour));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC days */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_day));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC months */
  trav = endstr - 1;
  if ((trav < 0) || !isdigit(temp[trav]))
    trav++;
  sscanf (temp+trav, "%2d", &(time->tm_month));

  /* cut out minutes and extra characters */
  endstr = trav-1;
  while ((endstr>=0) && !isdigit(temp[endstr])) endstr --;
  if (endstr < 0)
    return 0;
  temp [endstr+1] = '\0'; 

  /* parse UTC year */
  trav = endstr;
  while ((trav >= 0) && (endstr-trav < 4) && isdigit(temp[trav]))
    trav--;
  sscanf (temp+trav+1, "%4d", &(time->tm_year));

  free (temp);
  return 0;
} 


int cal2utc (utc_t *time, cal_t calendar)
{
  int days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int month;

  if (UTC_LEAPYEAR(calendar.tm_year)) {
    days_in_month[1] = 29;
  }

  time->tm_year = calendar.tm_year;
  time->tm_yday = 0;
  for (month=1; month<calendar.tm_month; month++) {
    time->tm_yday += days_in_month[month-1];
  }
  time->tm_yday += calendar.tm_day;
  time->tm_hour = calendar.tm_hour;
  time->tm_min = calendar.tm_min;
  time->tm_sec = calendar.tm_sec;

  return 0;
}

int utc2cal (cal_t *calendar, utc_t time)
{
  int leapyr;
  int day_of_year = time.tm_yday;
  int day_of_month;
  int month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int Month;
  int i = 0;

  if (time.tm_year%4==0) leapyr=1; else leapyr=0;  
  if (time.tm_year%100==0 && time.tm_year%400!=0) leapyr=0; 

  if (leapyr) month[1] = 29;

  i=0;
  while (day_of_year > 0)  {
    day_of_year -= month[i];
    i++;
  }
  Month = i;
  day_of_month = day_of_year + month[i-1];

  /*  sprintf(utdatestr,"%2d-%2d-%4d",day_of_month,Month,time.tm_year);
  if (day_of_month < 10) utdatestr[0] = '0';
  if (Month < 10)        utdatestr[3] = '0';*/

  return 0;
}


/* *************************************************************************
   utc2str -
   This function returns a string representation of the time contained
   in "time".  The format of this string is given in "fmt" and the
   string is returned in "tmstr".

   tmstr - the returned string
   time  - a utc_t struct
   fmt   - the format of the string returned in tmstr

   Any characters in the set {y, d, h, m, s} will be replaced
   by the respective character of the time.  For instance:

   if   "fmt"  == "yyyy-dddd-hhhh-mm-ss"
   and  "time" == tm_year = 1995
                  tm_yday = 273
                  tm_hour = 13
                  tm_min  = 14
                  tm_sec  = 30
   then "tmstr will be "1995-0273-0013-14-30"

   Notice that any extra characters will be filled with zeroes.  The
   charaters are filled so that the least significant digits of each
   value are enterred first,  so fmt == "yy" would return "95" in
   the above example.  All other characters are copied literally.
   NOTE: "tmstr" must point to an area as large as or larger than
   the space to which "fmt" points or a sementation fault may occur.
   ************************************************************************* */
char* utc2str (char* tmstr, utc_t time, const char* fmt)
{
   strcpy (tmstr, fmt);
   return (utcstrfill (tmstr, time, strlen(fmt)));
}

/* *************************************************************************
   utcstrfill - does the work for "utc2str" as described above.

   tmstr - a string of "numch" characters containing the format of
           the desired output string.  It's characters will be replaced
           with the string representation of "time" as described above.
   time  - the UTC time
   numch - the number of characters in "tmstr"
   ************************************************************************* */
char* utcstrfill (char* tmstr, utc_t time, int numch)
{
   int pos;                 /* position in "tmstr" */
   int yc, dc, hc, mc, sc;  /* These variables contain the powers of ten
                               by which characters are pulled off of the
                               respective components of the time.
                               ie. yc - year, dc - day, hc - hour ...  */
   yc = dc = hc = mc = sc = 1;

   for (pos = numch -1; pos >= 0; pos--)  {
      switch (tmstr[pos])  {
         case 'y':
            tmstr[pos] = (char) ((time.tm_year / yc) % 10) + '0';
            yc *= 10;
         break;
         case 'd':
            tmstr[pos] = (char) ((time.tm_yday / dc) % 10) + '0';
            dc *= 10;
         break;
         case 'h':
            tmstr[pos] = (char) ((time.tm_hour / hc) % 10) + '0';
            hc *= 10;
         break;
         case 'm':
            tmstr[pos] = (char) ((time.tm_min / mc) % 10) + '0';
            mc *= 10;
         break;
         case 's':
            tmstr[pos] = (char) ((time.tm_sec / sc) % 10) + '0';
            sc *= 10;
         break;
         default:
         break;
      }
   }
   return (tmstr);
}

/* *************************************************************************
   utc_diff - returns the number of seconds between
              time1 and time2 (ie. time2 - time1).
   ************************************************************************* */
int utc_diff (utc_t time1, utc_t time2)
{
   while (time2.tm_year > time1.tm_year)  {
#ifdef UTC_DEBUG
         printf ("1: Julian Days in %i: %i\n", time2.tm_year -1,
                               UTC_JULIANDAYS (time2.tm_year -1) );
#endif
      time2.tm_year --;
      time2.tm_yday += UTC_JULIANDAYS (time2.tm_year);
   }
   while (time1.tm_year > time2.tm_year)  {
#ifdef UTC_DEBUG
         printf ("2: Julian Days in %i: %i\n", time1.tm_year -1,
                               UTC_JULIANDAYS (time1.tm_year -1) );
#endif
      time1.tm_year --;
      time1.tm_yday += UTC_JULIANDAYS (time1.tm_year);
   }

   return((time2.tm_sec - time1.tm_sec)
           + ((time2.tm_min - time1.tm_min) * 60)
           + ((time2.tm_hour - time1.tm_hour) * 3600)
           + ((time2.tm_yday - time1.tm_yday) * 86400));
}

/* *************************************************************************
   utc_inc - increments "time" by "seconds"
   ************************************************************************* */
int utc_inc (utc_t *time, int seconds)
{
   time->tm_sec += seconds;

   time->tm_min += time->tm_sec / 60;
   time->tm_sec = time->tm_sec % 60;

   time->tm_hour += time->tm_min / 60;
   time->tm_min = time->tm_min % 60;

   time->tm_yday += time->tm_hour / 24;
   time->tm_hour = time->tm_hour % 24;

   while (time->tm_yday >= UTC_JULIANDAYS(time->tm_year))  {
#ifdef UTC_DEBUG
         printf ("Julian day in %i: %i\n",time->tm_year,
                                          UTC_JULIANDAYS(time->tm_year));
#endif
      time->tm_year ++;
      time->tm_yday -= UTC_JULIANDAYS(time->tm_year);
   }
   return (0);
}

/* *************************************************************************
   utc_dec - decrements "time" by "seconds"
   ************************************************************************* */
int utc_dec (utc_t *time, int seconds)
{
   time->tm_sec -= seconds;

   if (time->tm_sec < 0)  {
      time->tm_min += ((time->tm_sec / 60) - 1);
      time->tm_sec = (time->tm_sec % 60) + 60;
   }

   if (time->tm_min < 0)  {
      time->tm_hour += ((time->tm_min / 60) - 1);
      time->tm_min = (time->tm_min % 60) + 60;
   }

   if (time->tm_hour < 0)  {
      time->tm_yday += ((time->tm_hour / 24) - 1);
      time->tm_hour = (time->tm_hour % 24) + 24;
   }

   while (time->tm_yday < 0)  {
      time->tm_year --;
      time->tm_yday += UTC_JULIANDAYS(time->tm_year);
   }

   return (0);
}

int utc_leapyear (int year)
{
   return ((year%4 == 0) && (year%400 != 0));
}

int utc_daysinyr (int year)
{
   return ( 364 + ( (utc_leapyear (year))?1:0 ) );
}

int str2LST (double* lst, char* timestr, float longitude)
{
  utc_t timeutc;

  str2utc (&timeutc, timestr);
  return utc2LST (lst, timeutc, longitude);
}

/* longitude is given in degrees */
int utc2LST (double* lst, utc_t timeutc, float longitude)
{
  double F1_YY, UT, GMST;
  double F1_96 = 6.5967564;     /* (hours) For 1996 see ASTRO ALMANAC pg B6 */
  double F1_97 = 6.6465521;     /* (hours) For 1997 see ASTRO ALMANAC pg B6 */
  double F2 = 0.0657098244;     /* (hours) */
  double F3 = 1.00273791;       /* (hours) */

  /* convert longitude in degrees to east_long in hours */
  double east_long = longitude * (24.0 / 360.0);

  if(timeutc.tm_year==1996) F1_YY = F1_96;
  else if(timeutc.tm_year==1997) F1_YY = F1_97;
  else if(timeutc.tm_year==1998) F1_YY = F1_97;
  else {
    fprintf(stderr,"Hello, this a utc2LST WARNING\n");
    fprintf(stderr,"The year %d is out of range\n",timeutc.tm_year);
    fprintf(stderr,"Enter revised conversion formula\n");
    fprintf(stderr,"from page B6 of current ASTRO ALMANAC\n");
    return -1;
  }

  UT = (double)timeutc.tm_hour + (double)timeutc.tm_min/60.0
    + (double)timeutc.tm_sec/3600.0;
  GMST = F1_YY+F2*(double)timeutc.tm_yday+F3*UT;

  *lst =  GMST + east_long;
  while (*lst<0.0) *lst+=24.0;
  if(*lst>=24.0) *lst=(float) fmod((double) *lst,24.0);
  return 0;
}

int utc2datestr (char* utdatestr, utc_t time)
{
  int leapyr;
  int day_of_year = time.tm_yday;
  int day_of_month;
  int month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
  int Month;
  int i = 0;

  if (time.tm_year%4==0) leapyr=1; else leapyr=0;  
  if (time.tm_year%100==0 && time.tm_year%400!=0) leapyr=0; 

  if (leapyr) month[1] = 29;

  i=0;
  while (day_of_year > 0)  {
    day_of_year -= month[i];
    i++;
  }
  Month = i;
  day_of_month = day_of_year + month[i-1];

  sprintf(utdatestr,"%2d-%2d-%4d",day_of_month,Month,time.tm_year);
  if (day_of_month < 10) utdatestr[0] = '0';
  if (Month < 10)        utdatestr[3] = '0';

  return 0;
}


