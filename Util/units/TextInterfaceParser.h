//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003-2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/TextInterfaceParser.h

#ifndef __TextInterfaceParser_h
#define __TextInterfaceParser_h

#include "Alias.h"
#include "Error.h"
#include "tostring.h"

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
    void set_value (const std::string& text)
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


  //! An array of Value interfaces
  class Parser : public Reference::Able
  {

  public:

    //! Default constructor
    Parser ();

    //! Get the named value
    std::string get_value (const std::string& name) const;

    //! Get the named value and update name for display purposes
    std::string get_name_value (std::string& name) const;

    //! Set the named value
    void set_value (const std::string& name, const std::string& value);

    //! Find the named value
    Value* find (const std::string& name, bool throw_exception = true) const;

    //! Return true if the named value is found
    bool found (const std::string& name) const;

    //! Return true if prefix:name is found
    bool found (const std::string& prefix, const std::string& name) const;

    //! Allow derived types to setup a Value instance before use
    virtual void setup (const Value*) { }

    //! Get the number of values
    unsigned get_nvalue () const;

    //! Get the name of the value
    std::string get_name (unsigned) const;

    //! Get the value
    std::string get_value (unsigned) const;

    //! Get the description of the value
    std::string get_description (unsigned) const;

    //! Process a command
    virtual std::string process (const std::string& command);
    
    //! Process a vector of commands
    virtual std::string process (const std::vector<std::string>& commands);

    //! Return the list of available values
    virtual std::string help (bool show_default_values = false,
			      bool show_header = false,
			      const char* indent = 0);

    //! Get the name of this interface
    virtual std::string get_interface_name () const { return ""; }

    //! Set the indentation that precedes the output of a call to process
    void set_indentation (const std::string& indent) { indentation = indent; }
    std::string get_indentation () const { return indentation; }

    //! Set the delimiter used to separate the elements of a container
    void set_delimiter (const std::string&);
    std::string get_delimiter () const { return delimiter; }

    //! Prefix output with "name="
    void set_prefix_name (bool flag) { prefix_name = flag; }

    //! Set aliases for value names
    void set_aliases (const Alias* alias) { aliases = alias; }

    //! Insert Parser into self
    void insert (Parser*);

    //! Import a nested interface
    void insert (const std::string& prefix, Parser*);

    //! Clear all nested interfaces
    void clean ();

  protected:

    //! The indentation that precedes the output of a call to process
    std::string indentation;

    //! The delimiter used to separate the elements of a container
    std::string delimiter;

    //! Maintain alphabetical order of parameter names
    bool alphabetical;

    //! Filter duplicate value names during import
    bool import_filter;

    //! Prefix "name=" when retrieving a value
    bool prefix_name;

    //! The aliases for the value names
    Reference::To<const Alias> aliases;

    //! Add a new value interface
    void add_value (Value* value);

    //! Remove the named value interface
    void remove (const std::string& name);

    //! Clean up invalid references in values vector
    void clean_invalid ();

    //! The vector of values
    std::vector< Reference::To<Value> > values;

  };

  //! returns true when name = prefix + ":" + value->get_name()
  bool matches (const std::string& name,
		const std::string& prefix, const Value* value);

}

#endif
