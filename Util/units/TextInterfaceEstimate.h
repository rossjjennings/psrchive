//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/TextInterfaceEstimate.h

#ifndef __TextInterfaceEstimate_h
#define __TextInterfaceEstimate_h

#include "TextInterfaceAttribute.h"
#include "Estimate.h"

#include "separate.h"
#include "stringtok.h"

namespace TextInterface
{
  //! Proxy enables attribute interface of Estimates in a vector
  /*! In this template: V is a vector of Estimate<T,U>; 
    Get is the method of V that returns const Estimate<T,U>; 
    and Size is the method of V that returns the number elements in V */
  template<class V, class T, class U, class Get, class Size>
  class VectorOfEstimate : public Attribute<V>
  {

  public:

    //! Construct from a pointer to element attribute interface
    VectorOfEstimate (const std::string& p, const std::string& t, Get g, Size s)
      { prefix = p; type = t; get = g; size = s; }

    //! Retun a newly constructed copy
    Attribute<V>* clone () const { return new VectorOfEstimate(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + "*:" + type; }

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
    std::string get_value (const V* ptr) const;

    //! Set the value of the attribute
    //! Set the value of the attribute
    void set_value (V*, const std::string&)
      { throw Error (InvalidState, "VectorOfEstimate::set_value",
                     prefix + " cannot be set"); }

    // void set_value (V* ptr, const std::string& value);

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

    //! Method of V that returns Estimate<T,U>
    Get get;

    //! Method of V that returns size of vector
    Size size;

    //! The name of the vector instance
    std::string prefix;

    //! Either "val" or "var"
    std::string type;

    //! The description of the attribute
    std::string description;

    //! The detailed description of the attribute
    std::string detailed_description;

    //! Range parsed from name during matches
    mutable std::string range;

  };

  //! ElementGet and ElementGetSet factory
  /*! Use this generator function object whenever the get and set
    methods to not match the patterns supplied by the add template
    methods of the TextInterface::To class */
  template<class C, class T, class U>
  class VAllocator< C, Estimate<T,U> >
  {

  public:

    //! Generate a new VectorOfEstimate instance
    template<class Get, class Size>
      void operator () (To<C>* to, const std::string& n, Get g, Size z)
      {
        to->add_value( new VectorOfEstimate<C,T,U,Get,Size> (n, "val", g, z) );
        to->add_value( new VectorOfEstimate<C,T,U,Get,Size> (n, "var", g, z) );
      }
   
    //! Generate a new ElementGet instance with description
    template<class Get, class Size>
      void operator () (To<C>* to, const std::string& n,
                   const std::string& d, Get g, Size z)
      {
        auto get = new VectorOfEstimate<C,T,U,Get,Size> (n, "val", g, z);
        get->set_description (d + " value");
        to->add_value( get );

        get = new VectorOfEstimate<C,T,U,Get,Size> (n, "var", g, z);
        get->set_description (d + " variance");
        to->add_value( get );
      }
  };

}

template<class V, class T, class U, class G, class S> 
std::string
TextInterface::VectorOfEstimate<V,T,U,G,S>::get_value (const V* ptr) const
{
  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*size)());
  std::string result;

  if (!this->parent)
    throw Error (InvalidState, "VectorOfEstimate["+prefix+"]", "no parent");

  for (unsigned i=0; i<ind.size(); i++)
  {
    // place a delimiter between elements
    if (i)
      result += this->parent->get_delimiter();

    // label the elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";

    Estimate<T,U> est = (ptr->*get)(ind[i]);

    if (type == "var")
      result += tostring( est.var );
    else
      result += tostring( est.val );
  }

  return result;
}

template<class V, class T, class U, class Get, class Size>
 bool TextInterface::VectorOfEstimate<V,T,U,Get,Size>::matches
  (const std::string& name) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::VectorOfEstimate::matches" << std::endl;
#endif

  std::string remainder;
  if (!match (prefix, name, &range, &remainder))
    return false;

  return remainder == type;
}

#endif

