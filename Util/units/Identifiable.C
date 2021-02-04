/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Identifiable.h"
#include "Error.h"
#include "stringcase.h"

#define _DEBUG 0
using namespace std;

//! Returns true if name matches identity (or aliases)
bool Identifiable::identify (const std::string& name)
{
  for (unsigned i=0; i<identities.size(); i++)
  {
#if _DEBUG
    cerr << "Identifiable::identify testing"
      	" id='" << identities[i] << "'" << endl;
#endif
    
    if (casecmp (identities[i], name))
    {
#if _DEBUG
      cerr << "Identifiable::identify name='" << name << "' matches"
	" id='" << identities[i] << "'" << endl;
#endif
      return true;
    }
  }
  
  return false;
}

void Identifiable::set_identity (const std::string& name)
{
  identities.insert (identities.begin(), name);
}

void Identifiable::add_alias (const std::string& name)
{
  identities.push_back (name);
}

//! Returns the identity of the object
std::string Identifiable::get_identity () const
{
  if (identities.size() == 0)
    throw Error (InvalidState, "Identifiable::get_identity",
                 "identity not set");

  return identities[0];
}

void Identifiable::set_description (const std::string& text)
{
  description = text;
}

//! Returns the identity of the object
std::string Identifiable::get_description () const
{
  return description;
}

