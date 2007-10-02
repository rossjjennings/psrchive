//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/Config.h,v $
   $Revision: 1.6 $
   $Date: 2007/10/02 05:19:32 $
   $Author: straten $ */

#ifndef __Pulsar_Config_h
#define __Pulsar_Config_h

#include "Configuration.h"
#include "TextInterface.h"

class CommandParser;

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
  template<typename T>
  class Option {

  public:

    Option (const std::string& name, const T& default_value)
    { init (0, name, "no description", default_value); }

    Option (T* ptr, const std::string& name, const T& default_value)
    { init (ptr, name, "no description", default_value); }

    Option (CommandParser* ptr,
	    const std::string& _name, const std::string& default_value);

    //! Cast to T& operator
    operator T& () { return *value; }

    //! Set equal to T operator
    T& operator = (const T& t) { *value = t; return *value; }

    std::string name;
    std::string description;
    T* value;

  protected:

    void init (T* ptr,
	       const std::string& name,
	       const std::string& description,
	       const T& default_value);

  };

  template<typename T>
  std::ostream& operator << (std::ostream& ostr, const Option<T>& option)
  { return ostr << *(option.value); }

  template<typename T>
  std::istream& operator >> (std::istream& istr, Option<T>& option)
  { return istr >> *(option.value); }

  template<typename T>
  TextInterface::Value* new_interface (Option<T>* option)
  { 
    return new TextInterface::Atom<T>( option->value,
				       option->name,
				       option->description );
  }

  // specialization for CommandParser classes
  template<>
  TextInterface::Value* new_interface (Option<CommandParser>* option);

  class Config::Interface : public TextInterface::Parser {

  public:

    ~Interface () { std::cerr << "Config::Interface destructor" << std::endl; }

    template<typename T>
    void add (Option<T>* option)
    {
      add_value ( new_interface(option) );
    }


  };

}


template<typename T>
void Pulsar::Option<T>::init (T* ptr,
			      const std::string& _name,
			      const std::string& _description,
			      const T& default_value)
{
#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init ptr=" << ptr << " name=" << _name
	    << " description=" << _description << " default=" << default_value
	    << std::endl;
#endif

  name = _name;
  description = _description;

  if (!ptr)
    value = new T;
  else
    value = ptr;

#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init get configuration" << std::endl;
#endif

  *value = Config::get_configuration()->get<T> (name, default_value);
  
#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init add to interface" << std::endl;
#endif

  Config::get_interface()->add (this);

#ifdef _DEBUG
  std::cerr << "Pulsar::Option<T>::init return" << std::endl;
#endif

}

template<typename T>
Pulsar::Option<T>::Option (CommandParser* ptr,
			   const std::string& _name,
			   const std::string& default_value)
{
  name = _name;
  description = "no description";
  value = ptr;

  value->parse( Config::get_configuration()->get<std::string> 
		(name, default_value) );
  
  Config::get_interface()->add (this);
}

#endif
