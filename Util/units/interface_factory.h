//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/interface_factory.h

#ifndef __interface_factory_h
#define __interface_factory_h

#include "separate.h"

// #define _DEBUG 1
#include "debug.h"

namespace TextInterface
{
  /*!

    T = base class
    C = container of pointers to derived classes

    The base class interface must include a method that returns a
    text-based interface to the properties of the class:

    TextInterface::Parser* get_interface ();

    The TextInterface::Parser::get_interface_name method will be used
    to select the appropriate derived class to be constructed.  The
    new derived class instance is created by calling the clone method.

  */

  template<typename T, typename C>
  T* factory (C& ptrs, std::string name_parse)
  {
    std::string name = stringtok (name_parse, ":");

    DEBUG("TextInterface::factory name=" << name);

    if (name == "0")
      return 0;
    
    Reference::To<T> result;

    std::string message;
    
    if (name == "help")
      message +=
        "\n\n"
        "Options:"
        "\n\n";
    
    for (auto ptr=ptrs.begin(); ptr != ptrs.end(); ptr++)
    {
      Reference::To<T> candidate = *ptr;
      Reference::To<TextInterface::Parser> interface = candidate->get_interface();

      if (name == "help")
      {
        message += interface->get_interface_name() + "\t"
          + interface->get_interface_description() + "\n"
          + interface->help (true, false, "   ");
      }
      else if (interface->get_interface_name() == name)
      {
        result = candidate->clone ();
      }

      interface = 0;
      candidate.release();

      if (result)
        break;
    }

    if (name == "help")
    {
      DEBUG("TextInterface::factory throwing HelpMessage = " << message);
      throw Error (HelpMessage, std::string(), message);
    }

    if (!result)
      throw Error (InvalidState, std::string(),
		   "no instance named '" + name + "'");
    
    DEBUG("TextInterface::factory options=" << name_parse);

    while (braced(name_parse))
    {
      DEBUG("TextInterface::factory removing brackets");
      name_parse.erase (name_parse.begin());
      name_parse.erase (name_parse.end()-1);
    }

    Reference::To<TextInterface::Parser> interface = result->get_interface();
    
    std::vector<std::string> options;
    standard_separation (options, name_parse);
    for (unsigned i=0; i<options.size(); i++)
    {
      DEBUG("TextInterface::factory option["<<i<<"]=" << options[i]);
      
      interface->process (options[i]);
    }

    interface = 0;
    return result.release();
  }
  
}
  
#endif
