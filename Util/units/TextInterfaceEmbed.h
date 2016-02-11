//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/TextInterfaceEmbed.h

#ifndef __TextInterfaceEmbed_h
#define __TextInterfaceEmbed_h

#include "TextInterfaceAttribute.h"
#include "separate.h"

namespace TextInterface
{
  //! Dynamically embeds the interface of something that can be got and set
  /*! In this template: C is a composite; Get is the method of C that
    returns something, Set is the method of C that sets something, and
    get_parser is the method that optionally returns the embedded interface
    to something. */
  template<class C, class Type, class Get, class Set>
  class OptionalInterface : public AttributeGetSet<C,Type,Get,Set>
  {

  public:
    
    //! Construct from a pointer to element attribute interface
    OptionalInterface (const std::string& t, Get g, Set s)
      : AttributeGetSet<C,Type,Get,Set> (t,g,s)
    { }

    //! Get the name of the attribute
    std::string get_name () const
    { return AttributeGetSet<C,Type,Get,Set>::get_name() + "[:<name>]"; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const;

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value);

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    virtual Parser* get_parser (const C* ptr) const = 0;

    //! Remainder parsed from name during matches
    mutable std::string remainder;
  };




  template<class C, class Type, class Get, class Set, class GetParser>
  class DirectInterface : public OptionalInterface<C,Type,Get,Set>
  {

  public:
    
    //! Construct from a pointer to element attribute interface
    DirectInterface (const std::string& t, Get g, Set s, GetParser p)
      : OptionalInterface<C,Type,Get,Set> (t,g,s)
    { get_parser_method = p; }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
    { return new DirectInterface(*this); }

  protected:

    //! Method of C that returns TextInterface::Parser
    GetParser get_parser_method;

    Parser* get_parser (const C* ptr) const
    { 
      return (const_cast<C*>(ptr)->*get_parser_method)();
    }
  };

  template<class C, class Type, class Get, class Set, class GetParser>
  class IndirectInterface : public OptionalInterface<C,Type,Get,Set>
  {

  public:
    
    //! Construct from a pointer to element attribute interface
    IndirectInterface (const std::string& t, Get g, Set s, GetParser p)
      : OptionalInterface<C,Type,Get,Set> (t,g,s)
    { get_parser_method = p; }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
    { return new IndirectInterface(*this); }

  protected:

    //! Method of C that returns TextInterface::Parser
    GetParser get_parser_method;

    Parser* get_parser (const C* ptr) const
    { 
      Type tptr = (const_cast<C*>(ptr)->*(this->get))();
      return (tptr->*get_parser_method)();
    }
  };

  template<class C, class Type>
  class EmbedAllocator 
  {
  public:
    //! Generate a new OptionalInterface instance with description
    template<class Get, class Set, class GetParser>
    OptionalInterface<C,Type,Get,Set>* 
    direct (const std::string& n, Get g, Set s, GetParser p)
    {
      return new DirectInterface<C,Type,Get,Set,GetParser> (n,g,s,p);
    }

    //! Generate a new OptionalInterface instance with description
    template<class Get, class Set, class GetParser>
    OptionalInterface<C,Type,Get,Set>* 
    indirect (const std::string& n, Get g, Set s, GetParser p)
    {
      return new IndirectInterface<C,Type,Get,Set,GetParser> (n,g,s,p);
    }
  };

  //! Dynamically embeds the interfaces of elements in a vector
  /*! In this template: V is a vector of elements; Get is the method of
    V that returns the element at the specified index; and Size is the
    method of V that returns the number of elements in it */
  template<class V, class Get, class Size>
  class VectorOfInterfaces : public Attribute<V>
  {

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

  //! Embedded interface factory for TextInterface::To<C>
  template<class C>
    class Embed
  {
  public:
    template <class G, class S>
    VectorOfInterfaces<C,G,S>* operator() (const std::string& name, G g, S s)
    {
      return new VectorOfInterfaces<C,G,S>(name, g, s);
    }
  };

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

template<class C, class T, class G, class S> 
std::string
TextInterface::OptionalInterface<C,T,G,S>::get_value (const C* ptr) const
{
  if (remainder == "")
    return AttributeGetSet<C,T,G,S>::get_value (ptr);

  Parser* parser = get_parser(ptr);

  if (remainder == "help")
    return "\n" + parser->help();
  else
    return parser->get_value (remainder);
}

template<class C, class T, class G, class S>
void TextInterface::OptionalInterface<C,T,G,S>::set_value (C* ptr,
							  const std::string& val)
{
  if (remainder == "")
    AttributeGetSet<C,T,G,S>::set_value (ptr, val);
  else
  {
    Parser* parser = get_parser(ptr);
    parser->set_value (remainder, val);
  }
}

template<class C, class T, class G, class S>
bool TextInterface::OptionalInterface<C,T,G,S>::matches
  (const std::string& text) const
{
#ifdef _DEBUG
  std::cerr << "TextInterface::OptionalInterface::matches"
    " text='" << text << "'" << std::endl;
#endif

  remainder = "";

  if (text == this->name)
    return true;

  if (text == this->name + "[:<name>]")
    return true;

  std::string range;
  if (!match (this->name, text, &range, &remainder))
    return false;

#ifdef _DEBUG
  std::cerr << "TextInterface::OptionalInterface::matches"
    " remainder='" << remainder << "'" << std::endl;
#endif

  if (remainder == "help")
    return true;

  if (!this->instance)
    return false;

#ifdef _DEBUG
  std::cerr << "TextInterface::OptionalInterface::matches"
    " getting Parser" << std::endl;
#endif

  Parser* parser = get_parser(this->instance);

#ifdef _DEBUG
  std::cerr << "TextInterface::OptionalInterface::matches"
    " got Parser" << std::endl;
#endif

  if (! parser->found (remainder))
    return false;

  return true;
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

    Reference::To<Parser> parser;
    parser = (const_cast<V*>(ptr)->*get)(ind[i])->get_interface();

    parser->set_delimiter( this->parent->get_delimiter() );

    if (remainder == "help")
      result += "\n" + parser->help();
    else
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

  if (remainder == "help")
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
