/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/ObsDescription.h"
#include "load_text.h"

using namespace std;

void Pulsar::FITSArchive::load_ObsDescription (fitsfile* fptr) try
{
  if (verbose)
    cerr << "load_ObsDescription entered" << endl;

  Reference::To<ObsDescription> description = new ObsDescription;

  load_text (fptr, "OBSDESCR", "DESCR", description.get(), verbose);

  add_extension( description );
}
catch (Error& error)
{
  throw error += "FITSArchive::load_ObsDescription";
}

