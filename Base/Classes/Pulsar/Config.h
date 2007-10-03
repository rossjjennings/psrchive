//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Config.h,v $
   $Revision: 1.10 $
   $Date: 2007/10/03 12:30:38 $
   $Author: straten $ */

#ifndef __Pulsar_Config_h
#define __Pulsar_Config_h

#include "Configuration.h"
#include "TextInterface.h"
#include "CommandParser.h"

namespace Pulsar {

  //! Stores PSRCHIVE configuration parameters
  class Config {

  public:

    //! Return the name of the installation directory
    static std::string get_home ();
    
    //! Return the name of the runtime directory
    static std::string get_runtime ();

    //! Return the configuration key/value pairs
    static Configuration* get_configuration ();

    class Interface;

    //! Return the text interface to the configuration parameters
    static Interface* get_interface ();

  protected:

    //! The global configuration file
    static Configuration* config;
    
    //! The global configuration interpreter
    static Interface* interface;

    //! Load the global configuration file
    static void load_config ();

  };

  //! Configuration option
  template<typename T, bool Mutable = true>
  class Option {

  public:

    Option (const std::string& name, const T& default_value,
	    const std::string& description = "none",
	    const std::string& detailed_description = "none" )
    { init (0, name, default_value, description, detailed_description); }

    Option (T* ptr, const std::string& name, const T& default_value,
	    const std::string& description = "none",
	    const std::string& detailed_description = "none")
    { init (ptr, name, default_value, description, detailed_description); }

    Option (CommandParser* ptr,
	    const std::string& _name, const std::string& default_value,
	    const std::string& description = "none",
	    const std::string& detailed_description = "none");

    //! Cast to T& operator
    operator T& () { return *value; }

    //! Set equal to T operator
    T& operator = (const T& t) { *value = t; return *value; }

    std::string name;
    std::string description;
    std::string detailed_description;
    T* value;

  protected:

    void init (T* ptr, const std::string& name, const T& default_value,
	       const std::string& description,
	       const std::string& detailed_description);

  };

  template<typename T, bool Mutable>
  std::ostream& operator << (std::ostream& ostr, const Option<T,Mutable>& o)
  { return ostr << *(o.value); }

  template<typename T, bool Mutable>
  std::istream& operator >> (std::istream& istr, Option<T,Mutable>& option)
  { return istr >> *(option.value); }

  template<typename T, bool Mutable>
  class OptionInterfaceGenerator {
  public:
    TextInterface::Value* operator() (Option<T,Mutable>* option)
    { 
      TextInterface::ValueGetSet<T>* value;
      value = new TextInterface::ValueGetSet<T>( option->value,
						 option->name,
						 option->description );
      value -> set_detailed_description ( option->detailed_description );
      return value;
    }
  };

  // specialization for read-only parameters
  template<typename T>
  class OptionInterfaceGenerator<T,false> {
    public:
    TextInterface::Value* operator() (Option<T,false>* option)
    { 
      TextInterface::ValueGet<T>* value;
      value = new TextInterface::ValueGet<T>( option->value,
					      option->name,
					      option->description );
      value -> set_detailed_description ( option->detailed_description );
      return value;
    }
  };


  // specialization for CommandParser classes
  template<bool Mutable>
  class OptionInterfaceGenerator<CommandParser,Mutable> {
    public:
    TextInterface::Value* operator() (Option<CommandParser,Mutable>* option)
    {
      return TextInterface::new_Interpreter( option->name,
		                             option->description,
					     option->detailed_description,
					     option->value,
					     &CommandParser::empty,
					     &CommandParser::parse );
    }
  };

  class Config::Interface : public TextInterface::Parser {

  public:

    Interface () { alphabetical = true; }

    template<typename T, bool Mutable>
    void add (Option<T,Mutable>* option)
    {
      OptionInterfaceGenerator<T,Mutable> generator;
      add_value ( generator(option) );
    }


  };

}


template<typename T,bool Mutable>
void Pulsar::Option<T,Mutable>::init (T* ptr, const std::string& _name,
				      const T& default_value,
				      const std::string& _description,
				      const std::string& _detailed_description)
{
#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init ptr=" << ptr << " name=" << _name
	    << " description=" << _description << " default=" << default_value
	    << std::endl;
#endif

  name = _name;
  description = _description;
  detailed_description = _detailed_description;

  if (!ptr)
    value = new T;
  else
    value = ptr;

#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init get configuration" << std::endl;
#endif

  *value = Config::get_configuration()->get (name, default_value);
  
#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init add to interface" << std::endl;
#endif

  Config::get_interface()->add (this);

#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init return" << std::endl;
#endif

}

template<typename T,bool Mutable>
Pulsar::Option<T,Mutable>::Option (CommandParser* ptr,
				   const std::string& _name,
				   const std::string& default_value,
				   const std::string& _description,
				   const std::string& _detailed_description)
{
  name = _name;
  description = _description;
  detailed_description = _detailed_description;
  value = ptr;

  value->parse( Config::get_configuration()->get (name, default_value) );
  
  Config::get_interface()->add (this);
}

#endif
