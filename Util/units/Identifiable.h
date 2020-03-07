
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

#include "ReferenceTo.h"

class Identifiable : public Reference::Able
{
  public:

  //! Returns true if name matches identity (or aliases)
  virtual bool identify (const std::string& name);

  //! Set the identity of the object
  virtual void set_identity (const std::string&);
 
  //! Returns the identity of the object
  virtual const std::string& get_identity () const;

  //! Set the description of the object
  virtual void set_description (const std::string&);

  //! Returns the description of the object
  virtual const std::string& get_description () const;

  //! Add an alias for the object
  virtual void add_alias (const std::string&);

  class Decorator;

 private:

  //! Primary identity and aliases
  std::vector<std::string> identities;

  //! The description of the object
  std::string description;

};

class Identifiable::Decorator : public Identifiable
{
  Reference::To<Identifiable> decorated;

public:

  Decorator (Identifiable* id) { decorated = id; }

  bool identify (const std::string& name)
  { return decorated->identify(name); }

  const std::string& get_identity () const
  { return decorated->get_identity (); }

  const std::string& get_description () const
  { return decorated->get_description (); }

};


#endif
