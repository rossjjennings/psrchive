#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// #include <iostream>

#define MPI
#include "MJD.h"

int main (int argc, char* argv[]) 
{
  MJD stuffit;
  char printhere[40];
  int verbose = 0;

  if (argc > 1) {
    if (strcmp (argv[1], "-h") == 0) {
      fprintf (stderr, "getMJD [date]\n");
      fprintf (stderr, "WHERE: date is a UTC of the form yyyy-ddd-hh:mm:ss\n");
      fprintf (stderr, "       date is optional. [default: now]\n");
      return 0;
    }
    if (verbose) {
      fprintf (stderr, "Converting '%s' to UTC and using as date", argv[1]);
    }
    utc_t utcdate;
    if (str2utc (&utcdate, argv[1]) < 0) {
      fprintf (stderr, "Error converting '%s' to UTC.\n", argv[1]);
      return -1;
    }
    fprintf (stderr, "Using UTC parsed: %s\n",
	       utc2str (printhere, utcdate, "yyyy-ddd-hh:mm:ss"));
    stuffit = MJD (utcdate);
  }
  else {
    time_t temp = time(NULL);
    struct tm date = *gmtime(&temp);
    fprintf (stderr, "Using today's date: %s\n", asctime(&date));
    stuffit = MJD (date);
  }

  printf ("%11.5f\n", stuffit.in_days());

  if (verbose) {
    utc_t  stuffback;
    stuffit.UTC (&stuffback);
    printf ("utc from MJD: %s\n", 
	    utc2str (printhere, stuffback, "yyyy-ddd-hh:mm:ss"));
  }

  // cerr << stuffit.printdays(15);

  return 0;
}


