#include <stdio.h>
#include <stdlib.h>

#include <time.h>

extern long timezone;

int main ()
{
  time_t calculated, backup;
  struct tm* date;
  char datestr [25];
  int out;

  calculated = time (NULL);
  backup = calculated;

  date = localtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling localtime");
    return -1;
  }

  if (backup != calculated)  {
    printf ("WARNING: localtime() modifies its time_t argument.!!\n");
    calculated = backup;
  }

  out = strftime (datestr, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("local: %s\n", datestr);

  if (backup != calculated)  {
    printf ("WARNING: strftime() modifies its time_t argument.!!\n");
    calculated = backup;
  }

  date = gmtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }
  out = strftime (datestr, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("UTC: %s\n", datestr);

  date = gmtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }

  calculated = mktime (date);
  calculated -= timezone;

  if (backup != calculated)  {
    printf ("ERROR: mktime() calculation provides unexpected result.!!\n");
  }
  date = gmtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling gmtime");
    return -1;
  }

  out = strftime (datestr, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("UTC: %s\n", datestr);

  date = localtime (&calculated);
  if (date == NULL)  {
    perror ("Error calling localtime");
    return -1;
  }

  out = strftime (datestr, 25, "%Y/%m/%d-%H:%M:%S", date);
  printf ("local: %s\n", datestr);

  printf ("difference between UTC and local time: %ld seconds.\n", timezone);

  if (backup != calculated)
    return -1;

  return 0;
}

