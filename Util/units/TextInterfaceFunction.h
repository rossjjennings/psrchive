//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2003 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/TextInterfaceFunction.h

#ifndef __TextInterfaceFunction_h
#define __TextInterfaceFunction_h

#include "TextInterfaceAttribute.h"

namespace TextInterface
{

  //! Get is function object that receives C* and returns some type
  /*! This class adapts a function on C* to behave like an attribute of C */
  template<class C, class Get>
  class GetFunction : public Attribute<C> {

  public:

    //! Constructor
    GetFunction (const std::string& _name, Get _get)
      : get (_get) { name = _name; }
    
    //! Copy constructor
    GetFunction (const GetFunction& copy)
      : get (copy.get) { name = copy.name; description = copy.description; }

    //! Return a clone
    Attribute<C>* clone () const { return new GetFunction(*this); }

    //! Get the name of the attribute
    std::string get_name () const { return name; }

    //! Get the description of the attribute
    std::string get_description () const { return description; }

    //! Get the description of the attribute
    void set_description (const std::string& d) { description = d; }

    //! Get the detailed description of the attribute
    std::string get_detailed_description () const
    { return detailed_description; }

    //! Get the detailed description of the attribute
    void set_detailed_description (const std::string& d)
    { detailed_description = d; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const
      { if (!ptr) return ""; return tostring( get(ptr) ); }

    //! Set the value of the attribute
    void set_value (C*, const std::string&)
      { throw Error (InvalidState, "GetFunction::set_value", 
		     name + " cannot be set"); }

    void set_modifiers (const std::string& modifiers) const
    {
      tostring_precision = fromstring<unsigned> (modifiers);
    }

    void reset_modifiers () const
    {
      tostring_precision = 0;
    }
    
  protected:

    //! The name of the attribute
    std::string name;

    //! The description of the attribute
    std::string description;

    //! The detailed description of the attribute
    std::string detailed_description;

    //! The get function object (functor)
    Get get;

  };

  //! Interface to a class attribute with an accessor and modifier methods
  template<class C, class Get, class Set>
  class GetSetFunction : public GetFunction<C, Get>
  {

  public:

    //! Constructor
    GetSetFunction (const std::string& _name, Get _get, Set _set)
      : GetFunction<C,Get> (_name, _get), set (_set) { }

    //! Return a clone
    Attribute<C>* clone () const { return new GetSetFunction(*this); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { set (ptr, fromstring<typename Set::second_argument_type> (value)); }

  protected:

    //! The set method
    Set set;
  };

}

#endif
