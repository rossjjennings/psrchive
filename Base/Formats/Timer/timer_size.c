#include <stdio.h>
#include "timer.h"

int main () {

  int size = sizeof(struct timer);

  if (size == TIMER_SIZE) {
    fprintf (stderr, "timer size ok\n");
    return 0;
  }

  fprintf (stderr, 
	   "****************************************************\n"
	   "\n"
	   "WARNING timer header struct has wrong size!\n"
	   "\n"
	   "sizeof(timer) == %d\n"
	   "TIMER_SIZE    == %d\n"
	   "\n"
	   "****************************************************\n",
	   size, TIMER_SIZE);

  return -1;
}

