#include <stdio.h>
#include "baseband_header.h"

int main () {

  int size = sizeof(baseband_header);

  if (size == BASEBAND_HEADER_SIZE) {
    fprintf (stderr, "baseband_header size = %d ... ok\n", size);
    return 0;
  }

  fprintf (stderr, 
	   "****************************************************\n"
	   "\n"
	   "WARNING baseband_header struct has wrong size!\n"
	   "\n"
	   "sizeof(baseband_header) == %d\n"
	   "BASEBAND_HEADER_SIZE    == %d\n"
	   "\n"
	   "****************************************************\n",
	   size, BASEBAND_HEADER_SIZE);

  return -1;
}

