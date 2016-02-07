/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ObsDescription.h"
#include "strutil.h"

using namespace std;

//! Default constructor
Pulsar::ObsDescription::ObsDescription ()
  : Extension ("ObsDescription")
{
}

//! Copy constructor
Pulsar::ObsDescription::ObsDescription (const ObsDescription& extension)
  : Extension ("ObsDescription")
{
  operator=(extension);
}

Pulsar::ObsDescription* Pulsar::ObsDescription::clone () const
{
  return new ObsDescription (*this);
}


void Pulsar::ObsDescription::load (FILE* fptr)
{
  text = "";
  if (stringload (&text, fptr) < 0)
    throw Error (FailedSys, "Pulsar::ObsDescription::load",
		 "stringload");
}

//! Unload to an open stream
void Pulsar::ObsDescription::unload (FILE* fptr) const
{
  if (fputs (text.c_str(), fptr) == EOF)
    throw Error (FailedSys, "Pulsar::ObsDescription::unload", 
		 "fputs");
}

