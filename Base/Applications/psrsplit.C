/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "tostring.h"

#include <stdlib.h>
#include <unistd.h>

using namespace std;

void usage ()
{
  cerr << 
    "psrsplit - split a pulsar archive into one or more files\n"
    "\n"
    "psrsplit [options] filename\n"
    "options:\n"
    "  -n subint  number of sub-integrations per output file \n"
       << endl;
}

int main (int argc, char** argv) try 
{
  unsigned nsubint = 0;

  char c;
  while ((c = getopt(argc, argv, "n:hqvV")) != -1) 

    switch (c)  {

    case 'h':
      usage();
      return 0;
    case 'V':
      Pulsar::Archive::set_verbosity (3);
      break;
    case 'v':
      Pulsar::Archive::set_verbosity (2);
      break;
    case 'q':
      Pulsar::Archive::set_verbosity (0);
      break;

    case 'n':
      nsubint = atoi (optarg);
      break;

    } 


  if (optind >= argc)
  {
    cerr << "psrsplit: please specify filename" << endl;
    return -1;
  }

  if (nsubint == 0)
  {
    cerr << "psrsplit: please specify "
      "number of sub-integrations per output file" << endl;
    return -1;
  }

  string filename = argv[optind];

  Reference::To<Pulsar::Archive> archive;
  archive = Pulsar::Archive::load( filename );

  unsigned nsub = archive->get_nsubint();

  unsigned isplit = 0;
  unsigned isub=0;

  while( isub < nsub )
  {
    cerr << "psrsplit: extracting";
    vector<unsigned> subints;
    for (unsigned jsub=0; jsub < nsubint && isub < nsub; jsub++)
    {
      cerr << " " << isub;
      subints.push_back( isub );
      isub ++;
    }
    cerr << endl;

    Reference::To<Pulsar::Archive> sub_archive = archive->extract(subints);

    string new_filename = filename + "." + tostring(isplit);

    cerr << "psrsplit: writing " << new_filename << endl;

    sub_archive->unload( new_filename );

    // by reloading, the sub-integrations that have been loaded will be deleted
    archive = Pulsar::Archive::load( filename );

    isplit ++;
  }

  return 0;

}
catch (Error& er)
{
  cerr << "psrsplit: " << er << endl;
  return -1;
}

