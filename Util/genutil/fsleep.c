#include <sys/time.h>

void fsleep (double seconds)
{
  struct timeval t ;
  
  t.tv_sec = seconds;
  seconds -= t.tv_sec;
  t.tv_usec = seconds * 1e6;
  select (0, 0, 0, 0, &t) ;
}
