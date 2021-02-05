//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/identifiable_factory.h

#ifndef __identifiable_factory_h
#define __identifiable_factory_h

/*!

  T = base class
  C = container of pointers to derived classes
  
  The base class interface must include:

  1) an "identify" method that receives a string argument and returns
     true if it matches the identify of the object or any of its aliases

  2) a "get_identity" method that returns a string for the primary name
     of the object

  3) a "clone" method that returns a new instance of a derived class
  
*/

template<typename T, typename C>
T* identifiable_factory (C& ptrs, std::string name)
{

#ifdef _DEBUG
  std::cerr << "identifiable_factory name=" << name << std::endl;
#endif

  std::string message;

  if (name == "help")
    message += "\n\n" "Options:" "\n\n";

#ifdef _DEBUG
  std::cerr << "identifiable_factory ptrs=" << (void*) &ptrs << std::endl;
#endif
 
  for (auto ptr=ptrs.begin(); ptr != ptrs.end(); ptr++)
  {
#ifdef _DEBUG
    std::cerr << "ptr=" << (void*) (*ptr) << " id=" << (*ptr)->get_identity() << std::endl;
#endif

    if (name == "help")
    {
      message += (*ptr)->get_identity() +"\t"+ (*ptr)->get_description() +"\n";
    }
    else if ((*ptr)->identify(name))
    {
#ifdef _DEBUG
      std::cerr << "identifiable_factory calling clone" << std::endl;
#endif
      return (*ptr)->clone ();
    }
  }

  if (name == "help")
    throw Error (HelpMessage, "identifiable_factory", message);

  throw Error (InvalidState, "identifiable_factory",
	       "no instance named '" + name + "'");
}
  
#endif

