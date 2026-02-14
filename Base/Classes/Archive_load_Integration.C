/***************************************************************************
 *
 *   Copyright (C) 2007-2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

using namespace std;

Pulsar::Integration* Pulsar::Archive::load_Integration (unsigned isubint)
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::load_Integration" << endl;

  if (!__load_filename.length())
    throw Error (InvalidState, "Pulsar::Archive::load_Integration",
                 "internal error: instance not loaded from file");

  Reference::To<Integration> subint;
  subint = load_Integration (__load_filename.c_str(), isubint);

  // ensure that books are kept
  init_Integration (subint, true);

  /*
    bugs/509 - part 2
    When an Integration is first loaded, its ColdPlasmaHistory objects should always be initialized.
    (The init_Integration method initializes them onlyt if they do not already exist.)
   */

  bool overwrite_absolute = true;
  init_Dedisperse (subint, overwrite_absolute);
  init_DeFaraday (subint, overwrite_absolute);

  return subint.release();
}
