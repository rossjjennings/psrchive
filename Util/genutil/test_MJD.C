
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MPI
#include "MJD.h"

int main (int argc, char* argv[]) {
  MJD stuffit;
  utc_t  utcdate;
  utc_t  stuffback;
  char printhere[40];

  if (argc > 1) {
    fprintf (stderr, "Converting '%s' to UTC and using as date", argv[1]);
    if (str2utc (&utcdate, argv[1]) < 0) {
      fprintf (stderr, "Error converting '%s' to UTC.\n", argv[1]);
      return -1;
    }
    fprintf (stderr, "UTC parsed: %s\n",
	     utc2str (printhere, stuffback, "yyyy-ddd-hh:mm:ss"));
    stuffit = MJD (utcdate);
    return -1;
  }
  else {
    struct tm date  = *gmtime(time());
    fprintf (stderr, "Using today's date: %s\n", asctime(&date);
    stuffit = MJD (date);
  }

  printf ("MJD: %s\n", stuffit.printall());

  stuffit.UTC (&stuffback);
  printf ("utc from MJD: %s\n", 
	  utc2str (printhere, stuffback, "yyyy-ddd-hh:mm:ss"));

  return 0;
}


