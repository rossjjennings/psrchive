#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MJD.h"

int main ()
{
  struct tm* date;
  time_t present;
  char tempstr1 [256];
  char tempstr2 [256];

  present = time (NULL);

  date = gmtime (&present);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }
  strftime (tempstr1, 50, "%Y/%m/%d-%H:%M:%S", date);

  MJD mjd (*date);

  if (mjd.datestr (tempstr2, 50, "%Y/%m/%d-%H:%M:%S") == NULL) {
    perror ("Error calling MJD::datestr");
    return -1;
  }
  printf ("TM %s -> MJD %s -> TM %s\n", 
	  tempstr1, mjd.printdays(5).c_str(), tempstr2);

  if (strcmp (tempstr1, tempstr2))  {
    fprintf (stderr, "\nERROR!! TM1: %s does not match TM2: %s\n",
	     tempstr1, tempstr2);
    return -1;
  }
  printf ("\n**********************************************************\n");
  printf ("              MJD to TM operations test completed ok.");
  printf ("\n**********************************************************\n");

  return 0;
}
