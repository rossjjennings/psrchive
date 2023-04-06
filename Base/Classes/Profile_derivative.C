/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/DataExtension.h"

#include "FTransform.h"
#include "templates.h"

#include <memory>
#include <math.h>
#include <string.h>

using namespace std;

void Pulsar::Profile::derivative ()
{
  if (verbose)
    cerr << "Pulsar::Profile::derivative" 
	 << " nbin=" << get_nbin() << endl;

  unsigned nbin = get_nbin();
  float* amps = get_amps();

  FTransform::derivative (nbin, amps);
}

