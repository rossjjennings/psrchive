#include <iostream>
#include <unistd.h>

#include "poly.h"
#include "Error.h"

using namespace std;

int main (int argc, char** argv)
{
  string filename = "test.polyco";
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

  try {

  polyco data;

  if (data.load (filename) < 0)
    return -1;

  cout << data << endl;

  if (optind < argc)  {
    MJD test ("52054.43");
    data.phase (test);
  }

  }
  catch (Error& error)  {
    cerr << error << endl;
    return -1;
  }
  return 0;
}
