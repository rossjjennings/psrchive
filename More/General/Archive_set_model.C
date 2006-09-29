/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

void Pulsar::Archive::set_model (const polyco& new_model)
{
  if (!good_model (new_model))
    throw Error (InvalidParam, "Pulsar::Archive::set_model",
                 "supplied model does not span Integrations");

  // swap the old with the new
  Reference::To<polyco> oldmodel = model;
  model = new polyco (new_model);

  if (verbose == 3)
    cerr << "Pulsar::Archive::set_model apply the new model" << endl;

  // correct Integrations against the old model
  for (unsigned isub = 0; isub < get_nsubint(); isub++)
    apply_model (get_Integration(isub), oldmodel.ptr());

  // it may not be true the that supplied model was generated at runtime
  runtime_model = false;
}

