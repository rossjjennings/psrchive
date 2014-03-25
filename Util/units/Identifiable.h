
//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/Identifiable.h

#ifndef __Identifiable_H
#define __Identifiable_H

#include "ReferenceAble.h"

class Identifiable : public Reference::Able
{
  public:

  //! Returns true if name matches identity (or aliases)
  virtual bool identify (const std::string& name);

  //! Set the identity of the object
  virtual void set_identity (const std::string&);
 
  //! Returns the identity of the object
  virtual const std::string& get_identity () const;

  //! Add an alias for the object
  virtual void add_alias (const std::string&);

 private:

  //! Primary identity and aliases
  std::vector<std::string> identities;

};

#endif
