
/*
$Id: utc.h,v 1.1 1998/08/03 06:45:23 mbritton Exp $
$Log: utc.h,v $
Revision 1.1  1998/08/03 06:45:23  mbritton
Initial revision

Revision 1.2  1997/04/10 22:27:30  sasha
New function str2pos()

Revision 1.1  1996/11/18 19:53:57  sasha
Initial revision

Revision 1.1  1996/09/01 17:59:18  willem
Initial revision

*/

#ifndef UTC_H
#define UTC_H

/* #define UTC_DEBUG */

/* ******************
   UTC time structure
   ****************** */
typedef struct {
   int tm_sec;
   int tm_min;
   int tm_hour;
   int tm_yday;
   int tm_year;
} utc_t;

#ifdef __cplusplus
extern "C" {
#endif


int    str2utc    (utc_t *time, const char* str);
char*  utc2str    (char* str, utc_t time, const char* fmt);
char*  utcstrfill (char* str, utc_t time, int fill_chars);
int    utc_diff   (utc_t time1, utc_t time2);
int    utc_inc    (utc_t *time, int seconds);
int    utc_dec    (utc_t *time, int seconds);

int utc2LST (double* lst, utc_t timeutc, float east_longitude);
int str2LST (double* lst, char* timestr, float east_longitude);
int utc2datestr (char* utdatestr, utc_t time);


#ifdef __cplusplus
}
#endif

#define UTC_LEAPYEAR(year)   (((year)%4 == 0) && ((year)%400 != 0))
#define UTC_JULIANDAYS(year) ( 365 + ((UTC_LEAPYEAR(year))?1:0) )

#endif

