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
	   "****************************************************\n\n");

  fprintf (stderr, "sizeof(timer) == %d\n", size);
  fprintf (stderr, "TIMER_SIZE == %d\n", TIMER_SIZE);
  fprintf (stderr, "\n"
	   "****************************************************\n");

  return -1;
}

