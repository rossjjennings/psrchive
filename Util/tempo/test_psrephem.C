/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "psrephem.h"

using namespace std;

int main (int argc, char** argv)
{
  if (argc < 2) {
    fprintf (stderr, "USAGE: psrephem_test <psrname>\n");
    return -1;
  }

  string psr_name = argv[1];
  string tfile = psr_name + "_test1.eph";

  psrephem::verbose = 1;

  psrephem* eph = NULL;

  fprintf (stderr, "Loading TEMPO Parameters\n");

  if (argc > 2 && strcmp(argv[1],"-f")==0)  {
    fprintf (stderr, "Loading file: '%s'\n", argv[2]);
    eph = new psrephem (argv[2]);
  }
  else {
    fprintf (stderr, "Pulsar name: '%s'\n", argv[1]);
    eph = new psrephem (argv[1], 0);
  }

  fprintf (stderr, "Unloading to '%s'\n", tfile.c_str());
  if (eph->unload (tfile.c_str()) < 0)  {
    fprintf (stderr, "Error unloading ephemeris\n");
    return 0;
  }

  fprintf (stderr, "Test operator=\n");
  psrephem eph2 = *eph;

  tfile = psr_name + "_test2.eph";
  FILE* fptr = fopen (tfile.c_str(), "w+");
  if (fptr == NULL) {
    fprintf (stderr, "Could not open: %s\n", tfile.c_str());
    perror ("");
    return -1;
  }

  int nbytes;
  fprintf (stderr, "An example of writing an ephemeris to an open file\n");
  fprintf (fptr, "An example of writing an ephemeris to an open file\n");
  if ((nbytes = eph2.unload (fptr)) < 0)  {
    fprintf (stderr, "Error unloading ephemeris\n");
    return 0;
  }
  fprintf (fptr, "\n%d bytes unloaded\n", nbytes);

  fseek (fptr, 0L, SEEK_SET);
  fprintf (stderr, "An example of reading an ephemeris from an open file\n");
  char some [80];
  if (fgets (some, 80, fptr) == NULL)  {
    perror ("Could not read first bit of file.");
    return -1;
  }
  fprintf (stderr, "read from file: %s\n", some);

  if (eph->load (fptr, nbytes) < 0)  {
    fprintf (stderr, "Error loading ephemeris\n");
    return 0;
  }

  tfile = psr_name + "_test3.eph";
  if (eph->unload (tfile.c_str()) < 0)  {
    fprintf (stderr, "Error unloading ephemeris\n");
    return 0;
  }

  if ((nbytes = eph->unload (stdout)) < 0)  {
    fprintf (stderr, "Error unloading ephemeris\n");
    return 0;
  }

  delete eph;

  return 0;
}
