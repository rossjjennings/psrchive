/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/VariableTransformation.h"

using namespace Pulsar;

//! Default constructor
VariableTransformation::VariableTransformation ()
{
  built = false;
}

//! Set the Archive for which a tranformation will be computed
void VariableTransformation::set_archive (const Archive* _archive) 
{
  if (archive && archive != _archive)
    built = false;
 
  archive = _archive;
}

//! Set the sub-integration for which a tranformation will be computed
void VariableTransformation::set_subint (unsigned _subint)
{ 
  if (subint != _subint)
    built = false;
  
  subint = _subint;
}

//! Set the frequency channel for which a tranformation will be computed
void VariableTransformation::set_chan (unsigned _chan)
{
  if (chan != _chan)
    built = false;

  chan = _chan;
}

