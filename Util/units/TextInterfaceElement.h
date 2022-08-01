//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2003 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/TextInterfaceElement.h

#ifndef __TextInterfaceElement_h
#define __TextInterfaceElement_h

#include "TextInterfaceAttribute.h"

namespace TextInterface
{

  //! Interface to an array of attributes
  /*! In this template: C is a class that contains a vector of
    elements; Get is the accessor method of C that returns an element
    of the vector at a given index; and Size is the method of C that
    returns the number of elements in the vector. */
  template<class C, class Get, class Size>
  class ElementGet : public Attribute<C>
  {
  public:

    //! Constructor
    ElementGet (const std::string& _name, Get _get, Size _size)
      { name = _name; get = _get; size = _size; }

    //! Return a clone
    Attribute<C>* clone () const { return new ElementGet(*this); }

    //! Get the name of the attribute
    std::string get_name () const { return name + "*"; }

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
    std::string get_value (const C* ptr) const;

    //! Set the value of the attribute
    void set_value (C*, const std::string&)
      { throw Error (InvalidState, "ElementGet::set_value", 
		     name + " cannot be set"); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

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

    //! The get method
    Get get;

    //! The size method
    Size size;

    //! Range parsed from name during matches
    mutable std::string range;

  };

  //! Pointers to attribute get and set methods, Type C::Get() and C::Set(Type)
  template<class C, class Type, class Get, class Set, class Size>
    class ElementGetSet : public ElementGet<C, Get, Size> {

  public:

    //! Constructor
    ElementGetSet (const std::string& _name, Get _get, Set _set, Size _size)
      : ElementGet<C,Get,Size> (_name, _get, _size) { set = _set; }

    //! Return a clone
    Attribute<C>* clone () const { return new ElementGetSet(*this); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value);

  protected:

    //! The set method
    Set set;

  };

  //! ElementGet and ElementGetSet factory
  /*! Use this generator function object whenever the get and set
    methods to not match the patterns supplied by the add template
    methods of the TextInterface::To class */
  template<class C, class Type>
  class VAllocator {

  public:

    //! Generate a new ElementGet instance
    template<class Get, class Size>
      ElementGet<C,Get,Size>* 
      operator () (const std::string& n, Get g, Size z)
      { return new ElementGet<C,Get,Size> (n, g, z); }
    
    //! Generate a new ElementGetSet instance
    template<class Get, class Set, class Size>
      ElementGetSet<C,Type,Get,Set,Size>* 
      operator () (const std::string& n, Get g, Set s, Size z)
      { return new ElementGetSet<C,Type,Get,Set,Size> (n, g, s, z); }
    
    //! Generate a new ElementGet instance with description
    template<class Get, class Size>
      ElementGet<C,Get,Size>* 
      operator () (const std::string& n, const std::string& d, Get g, Size z)
      {
	ElementGet<C,Get,Size>* get = operator () (n,g,z);
	get->set_description (d); return get;
      }

    //! Generate a new ElementGetSet instance with description
    template<class Get, class Set, class Size>
      ElementGetSet<C,Type,Get,Set,Size>* 
      operator () (const std::string& n, const std::string& d,
		   Get g, Set s, Size z)
      {
	ElementGetSet<C,Type,Get,Set,Size>* get = operator () (n,g,s,z);
	get->set_description (d); return get;
      }

  };

  //! Label elements in ElementGetSet<C,E>::get_value
  extern bool label_elements;

}

template<class C,class Get,class Size>
 bool TextInterface::ElementGet<C,Get,Size>::matches
  (const std::string& var) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::ElementGet::matches" << std::endl;
#endif

  if (!match (name, var, &range))
    return false;

  return true;
}

template<class C, class G, class S> 
std::string TextInterface::ElementGet<C,G,S>::get_value (const C* ptr) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::ElementGet::get_value name=" << name 
	    << " range=" << range << std::endl;
#endif

  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*(size))());
  std::string result;

  if (!this->parent)
    throw Error (InvalidState, "ElementGet["+name+"]", "no parent");

  for (unsigned i=0; i<ind.size(); i++)
  {
    // place a delimiter between elements
    if (i)
      result += this->parent->get_delimiter();

    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";  // label the elements

    result += tostring( (ptr->*get)(ind[i]) );
  }

  return result;
}

template<class C, class T, class G, class S, class Z>
void TextInterface::ElementGetSet<C,T,G,S,Z>::set_value (C* ptr,
							 const std::string& v)
{
  std::vector<unsigned> ind;
  parse_indeces (ind, this->range, (ptr->*(this->size))());

  for (unsigned i=0; i<ind.size(); i++)
    (ptr->*set)(ind[i], fromstring<T>(v));
}

#endif
