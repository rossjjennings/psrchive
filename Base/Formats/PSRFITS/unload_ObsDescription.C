/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/ObsDescription.h"
#include "unload_text.h"

using namespace std;

void Pulsar::FITSArchive::unload (fitsfile* fptr, 
				  const Pulsar::ObsDescription* description)
{
  if (verbose > 1)
    cerr << "unload_ObsDescription entered verbose=" << verbose << endl;

  unload_text (fptr, "OBSDESCR", "DESCR", description, verbose > 1);
}
