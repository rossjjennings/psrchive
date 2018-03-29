//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/TextInterfaceValue.h

#ifndef __TextInterfaceValue_h
#define __TextInterfaceValue_h

#include "ReferenceAble.h"
#include "Error.h"

#include <string>

// #define _DEBUG 1

#ifdef _DEBUG
#include <iostream>
#endif

namespace TextInterface {

  class Parser;

  //! Text interface to a value of undefined type
  class Value : public Reference::Able
  {
  public:
    
    Value () { parent = 0; }

    //! Get the name of the value
    virtual std::string get_name () const = 0;

    //! Get the value as text
    virtual std::string get_value () const = 0;

    //! Set the value as text
    virtual void set_value (const std::string& value) = 0;

    //! Get the description of the value
    virtual std::string get_description () const = 0;

    //! Get the detailed description of the value
    virtual std::string get_detailed_description () const { return "none"; }

    //! Return true if the name argument matches the value name
    virtual bool matches (const std::string& name) const
    {
#ifdef _DEBUG
      std::cerr << "TextInterface::Value::matches" << std::endl;
#endif
      return name == get_name();
    }

    //! Parse any modifiers that will alter the behaviour of the output stream
    virtual void set_modifiers (const std::string&) const = 0;

    //! Reset any output stream modifiers
    virtual void reset_modifiers () const = 0;

    virtual void set_parent (Parser* p) { parent = p; }

  protected:

    Parser* parent;
  };

  //! Read-only interface to a value of type T
  template<class T>
  class ValueGet : public Value {

  public:
    
    //! Set all attributes on construction
    ValueGet (T* ptr, const std::string& _name, const std::string& _describe)
      { value_ptr = ptr; name = _name; description = _describe; }

    //! Get the name of the value
    std::string get_name () const
      { return name; }

    //! Get the value as text
    std::string get_value () const
      { return tostring (*value_ptr); }

    //! Set the value as text
    void set_value (const std::string&)
      { throw Error (InvalidState, "ValueGet::set_value", 
		     name + " cannot be set"); }

    //! Get the description of the value
    std::string get_description () const
      { return description; }

    //! Set the description of the value
    void set_description (const std::string& text)
      { description = text; }
       
    //! Get the detailed description of the value
    std::string get_detailed_description () const
      { return detailed_description; }

    //! Set the detailed description of the value
    void set_detailed_description (const std::string& text)
      { detailed_description = text; }

  protected:

    std::string name;
    std::string description;
    std::string detailed_description;
    T* value_ptr;

  };

  //! Read and write interface to a value of type T
  template<class T>
  class ValueGetSet : public ValueGet<T> {

  public:
    
    //! Set all attributes on construction
    ValueGetSet (T* ptr, const std::string& name, const std::string& desc)
      : ValueGet<T> (ptr, name, desc) { }

    //! Set the value as text
    void set_value (const std::string& text)
    { *(this->value_ptr) = fromstring<T>(text); }

  };
}

#endif
