
#include <stdio.h>
#include <stdlib.h>

#define MPI
#include "MJD.h"

int main (int argc, char* argv[]) {
  MJD stuffit;
  utc_t  utcdate;
  utc_t  stuffback;
  char printhere[40];

  if (argc < 2) {
    printf ("USAGE: %s <yyyy-ddd-hh:mm:ss>\n", argv[0]);
    printf ("or date with similar structure.\n\n");
    return -1;
  }

  str2utc (&utcdate, argv[1]);

  stuffit = MJD (utcdate);

  printf ("MJD from utc: %s\n", stuffit.printall());

  stuffit.UTC (&stuffback, NULL);

  printf ("utc from MJD: %s\n", utc2str (printhere, stuffback, "yyyy-ddd-hh:mm:ss"));

  return 0;
}


