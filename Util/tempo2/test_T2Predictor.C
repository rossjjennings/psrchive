/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Predictor.h"
#include "Error.h"

using namespace std;
using namespace Pulsar;

int main (int argc, char** argv) try
{

  if (argc < 2)
  {
    cerr << "Please specify a tempo2 predictor file name" << endl;
    return -1;
  }

  cerr << "Reading T2 predictor from " << argv[1] << endl;

  FILE* fptr = fopen (argv[1], "r");
  if (!fptr)
  {
    cerr << "could not open " << argv[1] << endl;
    return -1;
  }
 
  Tempo2::Predictor* predictor = new Tempo2::Predictor;
  predictor->load (fptr);

  cerr << "loaded:" << endl;

  predictor->unload (stdout);

  return 0;
}
 catch (Error& e) {
   cerr << e << endl;
   return -1;
 }

