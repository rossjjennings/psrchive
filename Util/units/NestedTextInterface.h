//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/NestedTextInterface.h

#ifndef __NestedTextInterface_h
#define __NestedTextInterface_h

#include "TextInterface.h"

namespace TextInterface {

  class Nested : public Value
  {
  };

  //! Proxy enables value interfaces to be imported with a name
  class NestedValue : public Nested
  {
  public:
    
    //! Construct from a name and pointer to Value
    NestedValue (const std::string& pre, Value* val)
      { prefix = pre; value = val; }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + ":" + value->get_name(); }

    //! Get the description of the value
    std::string get_description () const
      { return value->get_description(); }

    //! Get the value of the value
    std::string get_value () const 
      { return value->get_value (); }

    //! Set the value of the value
    void set_value (const std::string& txt)
      { value->set_value (txt); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const
    { return TextInterface::matches (name, prefix, value); }

    //! Parse any modifiers that will alter the behaviour of the output stream
    void set_modifiers (const std::string& modifiers) const
    { value->set_modifiers (modifiers); }

    //! Reset any output stream modifiers
    void reset_modifiers () const
    { value->reset_modifiers (); }

    void set_parent (Parser* p)
    { Value::set_parent(p); value->set_parent(p); }

  protected:

    //! The parent value interface
    Reference::To< Value > value;

    //! The name of the value
    std::string prefix;

  };

}

#endif
