#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utc.h"

int main ()
{
  char tempstr1 [256];
  char tempstr2 [256];
  utc_t time;
  struct tm cal;

  strcpy (tempstr1,  "1997251043045");
  str2utc (&time, tempstr1);
  printf ("TIME %s  ->  %s\n\n", tempstr1,
	   utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));

  strcpy (tempstr1,  "345");
  str2utc (&time, tempstr1);
  printf ("TIME %s  ->  %s\n\n", tempstr1,
	   utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));

  strcpy (tempstr1,  "1997-51-4-3:45");
  str2utc (&time, tempstr1);
  printf ("TIME %s  ->  %s\n\n", tempstr1,
	   utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));

  strcpy (tempstr1,  "test1997hard251a4is30this45");
  str2utc (&time, tempstr1);
  printf ("TIME %s  ->  %s\n\n", tempstr1,
	   utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));

  strcpy (tempstr1,  "970309-094500");
  str2tm (&cal, tempstr1);
  tm2utc (&time, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1997-068-09:45:00"))  {
    fprintf (stderr, "\tdoes not match expected answer '1997-068-09:45:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "880309-094500");
  str2tm (&cal, tempstr1);
  tm2utc (&time, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1988-069-09:45:00"))  {
    fprintf (stderr, "\tdoes not match expected answer '1988-069-09:45:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "000309-000000");
  str2tm (&cal, tempstr1);
  tm2utc (&time, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "2000-069-00:00:00"))  {
    fprintf (stderr, 
	     "WARNING: tm2utc() or str2tm() is sensitive to Y2000 kludges!\n");
    fprintf (stderr, "\texpected answer '2000-069-00:00:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "20000309-000000");
  str2tm (&cal, tempstr1);
  tm2utc (&time, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "2000-069-00:00:00"))  {
    fprintf (stderr, "\tdoes not match expected answer '2000-069-00:00:00'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "960201_205958");
  str2tm (&cal, tempstr1);
  tm2utc (&time, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1996-032-20:59:58"))  {
    fprintf (stderr, "\tdoes not match expected answer '1996-032-20:59:58'\n");
    return -1;
  }
  printf ("\n");

  strcpy (tempstr1,  "960229_160535");
  str2tm (&cal, tempstr1);
  tm2utc (&time, cal);
  printf ("TIME %s  ->  %s\n", tempstr1,
           utc2str (tempstr2, time, "yyyy-ddd-hh:mm:ss"));
  if (strcmp (tempstr2, "1996-060-16:05:35"))  {
    fprintf (stderr, "\tdoes not match expected answer '1996-060-16:05:35'\n");
    return -1;
  }
  printf ("\n");

  return 0;
}
