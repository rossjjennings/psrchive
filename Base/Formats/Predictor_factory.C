/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Predictor.h"

#include "polyco.h"
#include "T2Predictor.h"
// #include "Pulsar/SimplePredictor.h"

#include "strutil.h"
#include <vector>
using namespace std;

//! Factory creates an instance of a new Predictor
Pulsar::Predictor* Pulsar::Predictor::factory (FILE* fptr)
{
  long current = ftell (fptr);

  vector< Reference::To<Predictor> > candidates;
  candidates.push_back (new polyco);
  candidates.push_back (new Tempo2::Predictor);
  // candidates.push_back (new Pulsar::SimplePredictor);

  for (unsigned i=0; i < candidates.size(); i++) {

    if (fseek (fptr, current, SEEK_SET) < 0)
      throw Error (FailedSys, "Pulsar::Predictor::factory", "fseek");

    try {
      candidates[i]->load (fptr);
      return candidates[i].release();
    }
    catch (Error& error) {
      if (verbose)
        cerr << "Pulsar::Predictor::factory " << error.get_message() << endl;
    }

  }
  throw Error (InvalidParam, "Pulsar::Predictor::factory",
	       "no Predictor recognizes contents of file");
}

Pulsar::Predictor* Pulsar::Predictor::factory (FILE* fptr, size_t nbytes)
{
  FILE* temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "Pulsar::Predictor::factor", "tmpfile");

  ::copy (fptr, temp, nbytes);

  Predictor* model = 0;

  try {
    rewind (temp);
    model = Pulsar::Predictor::factory (temp);
  }
  catch (Error& error) { }

  fclose (temp);
  return model;
}

size_t Pulsar::Predictor::nbytes (const Pulsar::Predictor* model)
{
  FILE* temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "Pulsar::Predictor::nbytes", "tmpfile");

  size_t nbytes = 0;
  try {
    model->unload (temp);
    nbytes = ftell(temp);
  }
  catch (Error& error)
    {
      if (verbose)
	cerr << "Pulsar::Predictor::nbytes " << error.get_message() << endl;
    }

  fclose (temp);
  return nbytes;
}
