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
  if (verbose > 2)
    cerr << "load_ObsDescription entered" << endl;

  Reference::To<ObsDescription> description = new ObsDescription;

  load_text (fptr, "OBSDESCR", "DESCR", description.get(), verbose>2);

  add_extension( description );
}
catch (Error& error)
{
  if (verbose >2 )
    cerr << "FITSArchive::load_ObsDescription " << error.get_message() << endl;
}

