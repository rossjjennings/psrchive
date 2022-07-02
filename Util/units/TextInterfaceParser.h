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

#include "TextInterfaceValue.h"

#include "Alias.h"
#include "Error.h"
#include "tostring.h"

// #define _DEBUG 1

#ifdef _DEBUG
#include <iostream>
#endif

namespace TextInterface
{

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

    //! Get a short description of this interface
    virtual std::string get_interface_description () const { return ""; }

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
