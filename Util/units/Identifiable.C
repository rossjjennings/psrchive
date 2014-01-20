/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Identifiable.h"
#include "stringcase.h"

//! Returns true if name matches identity (or aliases)
bool Identifiable::identify (const std::string& name)
{
  for (unsigned i=0; i<identities.size(); i++)
    if (casecmp (identities[i], name))
      return true;

  return false;
}

//! Returns the identity of the object
const std::string& Identifiable::get_identity () const
{
  return identities.at(0);
}

