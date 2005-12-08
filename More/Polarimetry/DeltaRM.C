#include "Pulsar/DeltaRM.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

Pulsar::DeltaRM::DeltaRM ()
{
}

Pulsar::DeltaRM::~DeltaRM ()
{
}

//! Set the archive from which to derive the refined rotation measure
void Pulsar::DeltaRM::set_data (Archive* archive)
{
  data = archive;
}

//! Refine the rotation measure estimate
void Pulsar::DeltaRM::refine ()
{
  if (!data)
    throw Error (InvalidState, "Pulsar::DeltaRM::refine", "no data");
}
