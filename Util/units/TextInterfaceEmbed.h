//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/EmbeddedTextInterface.h

#ifndef __EmbeddedTextInterface_h
#define __EmbeddedTextInterface_h

#include "TextInterfaceAttribute.h"
#include "separate.h"
#include "stringtok.h"

namespace TextInterface
{
  //! Dynamically embeds the interfaces of elements in a vector
  /*! In this template: V is a vector of elements; Get is the method of
    V that returns the element at the specified index; and Size is the
    method of V that returns the number of elements in it */
  template<class V, class Get, class Size>
  class VectorOfInterfaces : public Attribute<V> {

  public:
    
    //! Construct from a pointer to element attribute interface
    VectorOfInterfaces (const std::string& p, Get g, Size s)
      { prefix = p; get = g; size = s; }

    //! Copy constructor
    VectorOfInterfaces (const VectorOfInterfaces& copy)
      { get=copy.get; size=copy.size; prefix=copy.prefix; }

    //! Retun a newly constructed copy
    Attribute<V>* clone () const
      { return new VectorOfInterfaces(*this); }

    //! Get the name of the attribute
    std::string get_name () const
    { return prefix + "*:<name>"; }

    //! Get the description of the attribute
    std::string get_description () const
    { return "attribute names depend on vector element"; }

    //! Get the value of the attribute
    std::string get_value (const V* ptr) const;

    //! Set the value of the attribute
    void set_value (V* ptr, const std::string& value);

    //! Set the description of the attribute
    void set_description (const std::string& description) {}

    //! Set the detailed description of the attribute
    void set_detailed_description (const std::string& description) {}

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! Method of V that returns E*
    Get get;

    //! Method of V that returns size of vector
    Size size;

    //! The name of the vector instance
    std::string prefix;

    //! Range parsed from name during matches
    mutable std::string range;

    //! Remainder parsed from name during matches
    mutable std::string remainder;
  };

  template<class C, class G, class S>
  VectorOfInterfaces<C,G,S>* embed (const std::string& name, G g, S s)
  {
    return new VectorOfInterfaces<C,G,S>(name, g, s);
  }

  //! Proxy enables attribute interface of elements in a map
  /*! In this template: M is a map of key K to element E; 
    Get is the method of V that returns E* given K */
  template<class M, class K, class Get>
  class MapOfInterfaces : public Attribute<M>
  {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    MapOfInterfaces (const std::string& pre, Get g)
      { prefix = pre; get = g; }

    //! Copy constructor
    MapOfInterfaces (const MapOfInterfaces& copy)
      { prefix=copy.prefix; get=copy.get;  }

    //! Set the prefix to be added before attribute name
    
    //! Retun a newly constructed copy
    Attribute<M>* clone () const
      { return new MapOfInterfaces(*this); }

    //! Get the name of the attribute
    std::string get_name () const
    { return prefix + "?:<name>"; }

    //! Get the description of the attribute
    std::string get_description () const
    { return "attribute names depend on map element"; }

    //! Get the value of the attribute
    std::string get_value (const M* ptr) const;

    //! Set the value of the attribute
    void set_value (M* ptr, const std::string& value);

    //! Set the description of the attribute
    void set_description (const std::string& description) {}

    //! Set the detailed description of the attribute
    void set_detailed_description (const std::string& description) {}

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! Method of M that returns element, given K
    Get get;

    //! The name of the map instance
    std::string prefix;

    //! Range parsed from name during matches
    mutable std::string range;

    //! Remainder parsed from name during matches
    mutable std::string remainder;

    //! Worker function parses keys for get_value and set_value
    void get_indeces (std::vector<K>& keys,
		      const std::string& param) const;

  };

}

template<class V, class G, class S> 
std::string
TextInterface::VectorOfInterfaces<V,G,S>::get_value (const V* ptr) const
{
  std::vector<unsigned> ind;
  parse_indeces (ind, this->range, (ptr->*(this->size))());
  std::string result;

  if (!this->parent)
    throw Error (InvalidState, "VectorOfInterfaces["+prefix+"]", "no parent");

  for (unsigned i=0; i<ind.size(); i++)
  {
    // place a delimiter between elements
    if (i)
      result += this->parent->get_delimiter();

    // label the elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";

    Reference::To<Parser> parser = (const_cast<V*>(ptr)->*get)(ind[i])->get_interface();

    parser->set_delimiter( this->parent->get_delimiter() );
    result += parser->get_value (remainder);
  }

  return result;
}

template<class V, class G, class S>
void TextInterface::VectorOfInterfaces<V,G,S>::set_value (V* ptr,
							  const std::string& val)
{
  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*size)());

  for (unsigned i=0; i<ind.size(); i++)
  {
    Reference::To<Parser> parser = (ptr->*get)(ind[i])->get_interface();
    parser->set_value (remainder, val);
  }
}


template<class C,class Get,class Size>
 bool TextInterface::VectorOfInterfaces<C,Get,Size>::matches
  (const std::string& name) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::VectorOfInterfaces::matches" << std::endl;
#endif

  if (!match (prefix, name, &range, &remainder))
    return false;

  if (remainder == "<name>")
    return true;

  if (!this->instance)
    return false;

  std::vector<unsigned> ind;
  parse_indeces (ind, range, (this->instance->*size)());

  for (unsigned i=0; i<ind.size(); i++)
  {
    Parser* parser = (const_cast<C*>(this->instance)->*get)(ind[i])->get_interface();
    if (! parser->found (remainder))
      return false;
  }
  return true;
}


template<class M, class K, class G> 
std::string
TextInterface::MapOfInterfaces<M,K,G>::get_value (const M* ptr) const
{
  std::vector<K> ind;
  get_indeces (ind, range);
  std::string result;

  if (!this->parent)
    throw Error (InvalidState, "MapOfInterfaces["+prefix+"]", "no parent");

  for (unsigned i=0; i<ind.size(); i++)
  {
    // place a delimiter between elements
    if (i)
      result += this->parent->get_delimiter();

    // label the elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";

    Reference::To<Parser> parser = (const_cast<M*>(ptr)->*get)(ind[i])->get_interface();
    parser->set_delimiter( this->parent->get_delimiter() );
    result += parser->get_value (remainder);
  }

  return result;
}

template<class M, class K, class G>
void TextInterface::MapOfInterfaces<M,K,G>::set_value (M* ptr,
						    const std::string& val)
{
  std::vector<K> ind;
  get_indeces (ind, range);

  for (unsigned i=0; i<ind.size(); i++)
  {
    Reference::To<Parser> parser = (ptr->*get)(ind[i])->get_interface();
    parser->set_value (remainder, val);
  }
}

template<class M, class K, class G>
void
TextInterface::MapOfInterfaces<M,K,G>::get_indeces (std::vector<K>& indeces,
						 const std::string& par) const
{
#ifdef _DEBUG
  std::cerr << "MapOfInterfaces::get_indeces " << par << std::endl;
#endif

  std::string::size_type length = par.length();

  std::string range = par;

  if (prefix.length()) {
    if (par[0] != '[' || par[length-1] != ']')
      return;
    range = par.substr (1, length-2);
  }
  else if (par == "?")
    return;

  std::vector<std::string> key_str;
  separate (range, key_str, ", ");

  indeces.resize (key_str.size());
  for (unsigned i=0; i<key_str.size(); i++)
    indeces[i] = fromstring<K>(key_str[i]);
}

template<class M, class K, class G>
 bool TextInterface::MapOfInterfaces<M,K,G>::matches
  (const std::string& name) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::MapOfInterfaces::matches" << std::endl;
#endif

  if (!match (prefix, name, &range, &remainder))
    return false;

  if (remainder == "<name>")
    return true;

  if (!this->instance)
    return false;

  std::vector<K> ind;
  get_indeces (ind, range);

  for (unsigned i=0; i<ind.size(); i++)
  {
    Parser* parser = (const_cast<M*>(this->instance)->*get)(ind[i])->get_interface();
    if (! parser->found (remainder))
      return false;
  }
  return true;
}

#endif
