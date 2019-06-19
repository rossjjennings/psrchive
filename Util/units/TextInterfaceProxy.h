//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2003 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/TextInterfaceProxy.h

#ifndef __TextInterfaceProxy_h
#define __TextInterfaceProxy_h

#include "TextInterfaceAttribute.h"
#include "TextInterfaceAdapter.h"

#include "separate.h"
#include "stringtok.h"

namespace TextInterface
{
  //! Proxy enables attribute interface from parent to be used by child
  /*! In this template: C is a P */
  template<class C, class P>
  class IsAProxy : public AttributeAdapter<C,P>
  {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    IsAProxy (const Attribute<P>* pa) 
      { this->value = pa->clone(); }

    //! Copy constructor
    IsAProxy (const IsAProxy& copy)
      { this->value = copy.value->clone(); }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
      { return new IsAProxy(*this); }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const 
    { if (!ptr) return "null";
      return this->value->get_value (ptr); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
    { if (ptr) this->value->set_value (ptr, value); }
  };

  //! Proxy enables attribute interface of member to be used by class
  /*! In this template: C has a M; Get is the method of C that returns M* */
  template<class C, class M, class Get>
  class HasAProxy : public AttributeAdapter<C,M>
  {

  public:

    //! Construct from a pointer to member attribute interface
    HasAProxy (const std::string& pre, const Attribute<M>* pa, Get g)
      { prefix = pre; this->value = pa->clone(); get = g; }

    //! Copy constructor
    HasAProxy (const HasAProxy& copy)
      { this->value=copy.value->clone(); get=copy.get; prefix=copy.prefix; }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
      { return new HasAProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { 
	if (prefix.length())
	  return prefix + ":" + this->value->get_name();
	else
	  return this->value->get_name();
      }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const 
    { if (!ptr) return "null";
      return this->value->get_value ((const_cast<C*>(ptr)->*get)()); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& val)
    { if (ptr) this->value->set_value ((ptr->*get)(), val); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const
    { return TextInterface::matches (name, prefix, this->value); }

  protected:

    //! Method of C that returns M*
    Get get;

    //! The name of the M attribute within C
    std::string prefix;
  };

  //! Proxy enables attribute interface of elements in a vector
  /*! In this template: V is a vector of E; Get is the method of V
    that returns E*; and Size is the method of V that returns the number
    of elements in it */
  template<class V, class E, class Get, class Size>
  class VectorOfProxy : public AttributeAdapter<V,E>
  {

  public:
    
    //! Construct from a pointer to element attribute interface
    VectorOfProxy (const std::string& p, const Attribute<E>* a, Get g, Size s)
      { prefix = p; this->value = a->clone(); get = g; size = s; }

    //! Copy constructor
    VectorOfProxy (const VectorOfProxy& copy)
      { this->value=copy.value->clone(); 
	get=copy.get; size=copy.size; prefix=copy.prefix; }

    //! Retun a newly constructed copy
    Attribute<V>* clone () const
      { return new VectorOfProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + "*:" + this->value->get_name(); }

    //! Get the value of the attribute
    std::string get_value (const V* ptr) const;

    //! Set the value of the attribute
    void set_value (V* ptr, const std::string& value);

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

  };


  //! Proxy enables attribute interface of elements in a map
  /*! In this template: M is a map of key K to element E; 
    Get is the method of V that returns E* given K */
  template<class M, class K, class E, class Get>
  class MapOfProxy : public AttributeAdapter<M,E>
  {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    MapOfProxy (const std::string& pre, const Attribute<E>* pa, Get g)
      { prefix = pre; this->value = pa->clone(); get = g; }

    //! Copy constructor
    MapOfProxy (const MapOfProxy& copy)
      { prefix=copy.prefix; this->value=copy.value->clone(); get=copy.get;  }

    //! Set the prefix to be added before attribute name
    
    //! Retun a newly constructed copy
    Attribute<M>* clone () const
      { return new MapOfProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + "?:" + this->value->get_name(); }

    //! Get the value of the attribute
    std::string get_value (const M* ptr) const;

    //! Set the value of the attribute
    void set_value (M* ptr, const std::string& value);

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! Method of M that returns E*, given K
    Get get;

    //! The name of the map instance
    std::string prefix;

    //! Range parsed from name during matches
    mutable std::string range;

    //! Worker function parses keys for get_value and set_value
    void get_indeces (std::vector<K>& keys,
		      const std::string& param) const;

  };

}

template<class V, class E, class G, class S> 
std::string
TextInterface::VectorOfProxy<V,E,G,S>::get_value (const V* ptr) const
{
  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*size)());
  std::string result;

  if (!this->parent)
    throw Error (InvalidState, "VectorOfProxy["+prefix+"]", "no parent");

  for (unsigned i=0; i<ind.size(); i++)
  {
    // place a delimiter between elements
    if (i)
      result += this->parent->get_delimiter();

    // label the elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";

    E* element = (const_cast<V*>(ptr)->*get)(ind[i]);
#ifdef _DEBUG
    std::cerr << "VectorOfProxy[" << prefix << "]::get_value (" 
	      << ptr << ") element=" << element << std::endl;
#endif
    result += this->value->get_value (element);
  }

  return result;
}

template<class V, class E, class G, class S>
void TextInterface::VectorOfProxy<V,E,G,S>::set_value (V* ptr,
						       const std::string& val)
{
  std::vector<unsigned> ind;
  parse_indeces (ind, range, (ptr->*size)());

  for (unsigned i=0; i<ind.size(); i++) {
    E* element = (ptr->*get)(ind[i]);
#ifdef _DEBUG
    std::cerr << "VectorOfProxy[" << prefix << "]::set_value (" 
	      << ptr << "," << val << ")" << std::endl;
#endif
    this->value->set_value (element, val);
  }
}


template<class C,class M,class Get,class Size>
 bool TextInterface::VectorOfProxy<C,M,Get,Size>::matches
  (const std::string& name) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::VectorOfProxy::matches" << std::endl;
#endif

  std::string remainder;
  if (!match (prefix, name, &range, &remainder))
    return false;

  return this->value->matches (remainder);
}



template<class M, class K, class E, class G> 
std::string
TextInterface::MapOfProxy<M,K,E,G>::get_value (const M* ptr) const
{
  std::vector<K> ind;
  get_indeces (ind, range);
  std::string result;

  if (!this->parent)
    throw Error (InvalidState, "MapOfProxy["+prefix+"]", "no parent");

  for (unsigned i=0; i<ind.size(); i++)
  {
    // place a delimiter between elements
    if (i)
      result += this->parent->get_delimiter();

    // label the elements
    if (label_elements && ind.size() > 1)
      result += tostring(ind[i]) + ")";

    E* element = (const_cast<M*>(ptr)->*get) (ind[i]);
    if (element)
      result += this->value->get_value (element);
  }

  return result;
}

template<class M, class K, class E, class G>
void TextInterface::MapOfProxy<M,K,E,G>::set_value (M* ptr,
						    const std::string& val)
{
  std::vector<K> ind;
  get_indeces (ind, range);

  for (unsigned i=0; i<ind.size(); i++)
    this->value->set_value ((ptr->*get)(ind[i]), val);
}

template<class M, class K, class E, class G>
void
TextInterface::MapOfProxy<M,K,E,G>::get_indeces (std::vector<K>& indeces,
						 const std::string& par) const
{
#ifdef _DEBUG
  std::cerr << "MapOfProxy::get_indeces " << par << std::endl;
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

template<class M, class K, class E, class G>
 bool TextInterface::MapOfProxy<M,K,E,G>::matches
  (const std::string& name) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::MapOfProxy::matches" << std::endl;
#endif

  std::string remainder;
  if (!match (prefix, name, &range, &remainder))
    return false;

  return this->value->matches (remainder);
}

#endif
