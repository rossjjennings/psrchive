#include <iostream>
#include <unistd.h>

#include "poly.h"

int main (int argc, char** argv)
{
  string filename = "polyco.dat";
  bool verbose = false;

  int c;
  while ((c = getopt(argc, argv, "hm:v")) != -1) {
    switch (c)  {
    case 'h':
      return 0;
    case 'v':
      polyco::verbose = true;
      verbose = true;
      break;
    }
  }

  if (optind < argc)
    filename = argv[optind];

  polyco data;

  data.load (filename);

  cout << data << endl;
}
