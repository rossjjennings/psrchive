//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/units/Configuration.h,v $
   $Revision: 1.5 $
   $Date: 2007/08/19 19:55:49 $
   $Author: straten $ */

#ifndef __Configuration_h
#define __Configuration_h

#include "tostring.h"
#include <vector>
#include <iostream>
#include <sstream>

//! Stores keyword-value pairs from a configuration file
/*! The Configuration class enables convenient, distributed access to
  configuration parameters stored in a simple text file */
class Configuration
{
public:

  //! Construct from the specified file
  Configuration (const char* filename = 0);

  //! Load the configuration from the specified file
  void load (const std::string& filename);

  //! Keyword-value pair
  class Entry;

  //! Get the value for the specified key
  template<typename T> T get (const std::string& key, T default_value) const;

  //! Find the entry with the specified key
  Entry* find (const std::string& key) const;

protected:

  std::vector<Entry> entries;

};

class Configuration::Entry {
public:
  Entry (std::string& k, std::string& v) { key=k; value=v; }
  std::string key;
  std::string value;
};

//! Get the value for the specified key
template<typename T> 
T Configuration::get (const std::string& key, T default_value) const
{
  Entry* entry = find (key);
  if (entry) {

#ifdef _DEBUG
    std::cerr << "Configuration::get found entry->value=" << entry->value 
	      << std::endl;
#endif

    /* IMPORTANT NOTE:
       cannot call fromstring because it uses a global variable that may
       not be initialized at the time that this template function is called
    */

    std::istringstream ist;
    ist.str (entry->value);

    T value;
    ist >> value;

#ifdef _DEBUG
    std::cerr << "Configuration::get found " << key 
                << " = " << value << std::endl;
#endif

    return value;
  }

#ifdef _DEBUG
  std::cerr << "Configuration::get default " << key 
            << " = " << default_value << std::endl;
#endif

  return default_value;
}

#endif

