/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/FITSArchive.h"
#include "Predictor.h"

using namespace std;

// load a tempo polyco, defined in load_polyco.C
Pulsar::Predictor* load_polyco (fitsfile* fptr, FITSPolyco*, bool verbose);

// load a tempo2 predictor, defined in load_T2Predictor.C
Pulsar::Predictor* load_T2Predictor (fitsfile* fptr, bool verbose);

void Pulsar::FITSArchive::load_Predictor (fitsfile* fptr)
{
  hdr_model = model = load_polyco (fptr, &extra_polyco, verbose > 2);

#ifdef HAVE_TEMPO2
  if (!model)
    // Load the Tempo2 Predictor, if any
    load_T2Predictor (fptr, verbose > 2);
#endif
}

