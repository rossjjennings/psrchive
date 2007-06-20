/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ObsExtension.h"
#include "Pulsar/ObsExtensionTI.h"

//! Default constructor
Pulsar::ObsExtension::ObsExtension ()
    : Extension ("ObsExtension")
{
  observer = "UNSET";
  affiliation = "UNSET";
  project_ID = "UNSET";
  telescope = "UNSET";
}

//! Copy constructor
Pulsar::ObsExtension::ObsExtension (const ObsExtension& extension)
    : Extension ("ObsExtension")
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
{}


//! Ge text interface
Reference::To< TextInterface::Class > Pulsar::ObsExtension::get_text_interface()
{
  return new ObsExtensionTI( this );

}
