#include "Pulsar/ObsExtension.h"

//! Default constructor
Pulsar::ObsExtension::ObsExtension ()
{
  observer = "UNSET";
  affiliation = "UNSET";
  project_ID = "UNSET";
  telescope = "UNSET";
}

//! Copy constructor
Pulsar::ObsExtension::ObsExtension (const ObsExtension& extension)
{
  observer = extension.observer;
  affiliation = extension.affiliation;
  project_ID = extension.project_ID;
  telescope = extension.telescope;
}

//! Operator =
const Pulsar::ObsExtension&
Pulsar::ObsExtension::operator= (const ObsExtension& extension)
{
  observer = extension.observer;
  affiliation = extension.affiliation;
  project_ID = extension.project_ID;
  telescope = extension.telescope;

  return *this;
}

//! Destructor
Pulsar::ObsExtension::~ObsExtension ()
{
}
