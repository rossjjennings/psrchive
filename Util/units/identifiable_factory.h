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

  if (name == "help")
    std::cerr <<
      "\n\n"
      "Options:"
      "\n\n";
    
  for (typename C::iterator ptr=ptrs.begin(); ptr != ptrs.end(); ptr++)
    {
      if (name == "help")
	{
	  std::cerr << (*ptr)->get_identity() 
                    << "\t" << (*ptr)->get_description() << std::endl;
	}
      else if ((*ptr)->identify(name))
	{
	  return(*ptr)->clone ();
	}
    }

  if (name == "help")
  {
    std::cerr << std::endl;
    return 0;
  }

  throw Error (InvalidState, std::string(),
	       "no instance named '" + name + "'");
}
  
  
#endif
