/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __TextInterface_h
#define __TextInterface_h

#include "Reference.h"
#include "Error.h"
#include "tostring.h"
#include "stringtok.h"
#include "separate.h"

// #define _DEBUG 1

namespace TextInterface {

  //! Abstract base class of attribute text interface
  template<class C>
  class Attribute : public Reference::Able {

  public:
    
    //! Retun a newly constructed copy
    virtual Attribute* clone () const = 0;

    //! Get the name of the attribute
    virtual std::string get_name () const = 0;

    //! Get the description of the attribute
    virtual std::string get_description () const = 0;

    //! Get the value of the attribute
    virtual std::string get_value (const C* ptr) const = 0;

    //! Set the value of the attribute
    virtual void set_value (C* ptr, const std::string& value) = 0;

    //! Set the description of the attribute
    virtual void set_description (const std::string&) = 0;

    //! Return true if the name argument matches
    virtual bool matches (const std::string& name) const;

  };

  //! Proxy enables attribute interface from parent to be used by child
  /*! In this template: C is a P */
  template<class C, class P>
  class IsAProxy : public Attribute<C> {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    IsAProxy (const Attribute<P>* pa) { attribute = pa->clone(); }

    //! Copy constructor
    IsAProxy (const IsAProxy& copy)
      { attribute = copy.attribute->clone(); }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
      { return new IsAProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const 
      { return attribute->get_value (ptr); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { attribute->set_value (ptr, value); }

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<P> > attribute;

  };

  //! Proxy enables attribute interface of member to be used by class
  /*! In this template: C has a M; Get is the method of C that returns M* */
  template<class C, class M, class Get>
  class HasAProxy : public Attribute<C> {

  public:
    
    //! Construct from a pointer to member attribute interface
    HasAProxy (const std::string& pre, const Attribute<M>* pa, Get g)
      { prefix = pre; attribute = pa->clone(); get = g; }

    //! Copy constructor
    HasAProxy (const HasAProxy& copy)
      { attribute=copy.attribute->clone(); get=copy.get; prefix=copy.prefix; }

    //! Retun a newly constructed copy
    Attribute<C>* clone () const
      { return new HasAProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + ":" + attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const 
      { return attribute->get_value ((const_cast<C*>(ptr)->*get)()); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { attribute->set_value ((ptr->*get)(), value); }

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<M> > attribute;

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
  class VectorOfProxy : public Attribute<V> {

  public:
    
    //! Construct from a pointer to element attribute interface
    VectorOfProxy (const std::string& pre, const Attribute<E>* a, Get g, Size s)
      { prefix = pre; attribute = a->clone(); get = g; size = s; }

    //! Copy constructor
    VectorOfProxy (const VectorOfProxy& copy)
      { attribute=copy.attribute->clone(); 
	get=copy.get; size=copy.size; prefix=copy.prefix; }

    //! Retun a newly constructed copy
    Attribute<V>* clone () const
      { return new VectorOfProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + "*:" + attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const V* ptr) const;

    //! Set the value of the attribute
    void set_value (V* ptr, const std::string& value);

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<E> > attribute;

    //! Method of V that returns E*
    Get get;

    //! Method of V that returns size of vector
    Size size;

    //! The name of the vector instance
    std::string prefix;

    //! Range parsed from name during matches
    std::string range;

    //! Worker function parses indeces for get_value and set_value
    void get_indeces (const V* ptr, std::vector<unsigned>& indeces,
		      const std::string& param) const;

  };


  //! Proxy enables attribute interface of elements in a map
  /*! In this template: M is a map of key K to element E; 
    Get is the method of V that returns E* given K */
  template<class M, class K, class E, class Get>
  class MapOfProxy : public Attribute<M> {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    MapOfProxy (const std::string& pre, const Attribute<E>* pa, Get g)
      { prefix = pre; attribute = pa->clone(); get = g; }

    //! Copy constructor
    MapOfProxy (const MapOfProxy& copy)
      { prefix=copy.prefix; attribute=copy.attribute->clone(); get=copy.get;  }

    //! Set the prefix to be added before attribute name
    
    //! Retun a newly constructed copy
    Attribute<M>* clone () const
      { return new MapOfProxy(*this); }

    //! Get the name of the attribute
    std::string get_name () const
      { return prefix + "?:" + attribute->get_name(); }

    //! Get the description of the attribute
    std::string get_description () const
      { return attribute->get_description(); }

    //! Get the value of the attribute
    std::string get_value (const M* ptr) const;

    //! Set the value of the attribute
    void set_value (M* ptr, const std::string& value);

    //! Set the description of the attribute
    void set_description (const std::string& description)
      { attribute->set_description (description); }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

  protected:

    //! The parent attribute interface
    Reference::To< Attribute<E> > attribute;

    //! Method of M that returns E*, given K
    Get get;

    //! The name of the map instance
    std::string prefix;

    //! Range parsed from name during matches
    std::string range;

    //! Worker function parses keys for get_value and set_value
    void get_indeces (std::vector<K>& keys,
		      const std::string& param) const;

  };

  //! Pointer to attribute get method, C::Get(), that returns some type
  template<class C, class Get>
  class AttributeGet : public Attribute<C> {

  public:

    //! Constructor
    AttributeGet (const std::string& _name, Get _get)
      { name = _name; get = _get; }

    //! Copy constructor
    AttributeGet (const AttributeGet& copy)
      { name = copy.name; description = copy.description; get = copy.get; }

    //! Return a clone
    Attribute<C>* clone () const { return new AttributeGet(*this); }

    //! Get the name of the attribute
    std::string get_name () const { return name; }

    //! Get the description of the attribute
    std::string get_description () const { return description; }

    //! Get the description of the attribute
    void set_description (const std::string& d) { description = d; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const
      { if (!ptr) return ""; return tostring( (ptr->*get) () ); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { throw Error (InvalidState, "AttributeGet::set_value", 
		     name + " cannot be set"); }

  protected:

    //! The name of the attribute
    std::string name;

    //! The description of the attribute
    std::string description;

    //! The get method
    Get get;

  };

  //! Pointer to a Unary Function that receives C* and returns some type
  template<class C, class Unary>
  class UnaryGet : public Attribute<C> {

  public:

    //! Constructor
    UnaryGet (const std::string& _name, Unary _get)
      : get (_get) { name = _name; }
    
    //! Copy constructor
    UnaryGet (const UnaryGet& copy)
      : get (copy.get) { name = copy.name; description = copy.description; }

    //! Return a clone
    Attribute<C>* clone () const { return new UnaryGet(*this); }

    //! Get the name of the attribute
    std::string get_name () const { return name; }

    //! Get the description of the attribute
    std::string get_description () const { return description; }

    //! Get the description of the attribute
    void set_description (const std::string& d) { description = d; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const
      { if (!ptr) return ""; return tostring( get(ptr) ); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { throw Error (InvalidState, "UnaryGet::set_value", 
		     name + " cannot be set"); }

  protected:

    //! The name of the attribute
    std::string name;

    //! The description of the attribute
    std::string description;

    //! The get function object (functor)
    Unary get;

  };

  //! Pointers to attribute get and set methods, Type C::Get() and C::Set(Type)
  template<class C, class Type, class Get, class Set>
  class AttributeGetSet : public AttributeGet<C, Get> {

  public:

    //! Constructor
    AttributeGetSet (const std::string& _name, Get _get, Set _set)
      : AttributeGet<C,Get> (_name, _get) { set = _set; }

    //! Copy constructor
    AttributeGetSet (const AttributeGetSet& copy) 
      : AttributeGet<C,Get> (copy) { set = copy.set; }

    //! Return a clone
    Attribute<C>* clone () const { return new AttributeGetSet(*this); }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { (ptr->*set) (fromstring<Type>(value)); }

  protected:

    //! The set method
    Set set;

  };

  //! AttributeGet and AttributeGetSet factories
  /*! Use these factories whenever the get and set methods to not
    match the patterns supplied by the corresponding factories of the
    TextInterface::To class */
  template<class C, class Type>
  class Allocator {

  public:

    //! Generate a new AttributeGet instance
    template<class Get>
      AttributeGet<C,Get>* 
      named (const std::string& n, Get g)
      { return new AttributeGet<C,Get> (n, g); }
    
    //! Generate a new AttributeGetSet instance
    template<class Get, class Set>
      AttributeGetSet<C,Type,Get,Set>* 
      named (const std::string& n, Get g, Set s)
      { return new AttributeGetSet<C,Type,Get,Set> (n, g, s); }
    
    //! Generate a new AttributeGet instance with description
    template<class Get>
      AttributeGet<C,Get>* 
      described (const std::string& n, const std::string& d, Get g)
      {
	AttributeGet<C,Get>* get = named (n,g);
	get->set_description (d); return get;
      }

    //! Generate a new AttributeGetSet instance with description
    template<class Get, class Set>
      AttributeGetSet<C,Type,Get,Set>* 
      described (const std::string& n, const std::string& d, Get g, Set s)
      {
	AttributeGetSet<C,Type,Get,Set>* get = named (n,g,s);
	get->set_description (d); return get;
      }

  };

  //! Abstract base class of class text interface
  class Class : public Reference::Able {

  public:

    //! Process a command
    virtual std::string process (const std::string& command);

    //! Return the list of available attributes
    std::string help (bool show_default_values = false);

    //! Get the value of the attribute
    virtual std::string get_value (const std::string& name) const = 0;

    //! Set the value of the attribute
    virtual void set_value (const std::string& name, 
			    const std::string& value) = 0;

    //! Get the number of attributes
    virtual unsigned get_nattribute () const = 0;

    //! Get the name of the attribute
    virtual std::string get_name (unsigned) const = 0;

    //! Get the description of the attribute
    virtual std::string get_description (unsigned) const = 0;

  };


  //! Class text interface: an instance of C and a vector of Attribute<C>
  template<class C>
  class To : public Class {

  public:

    template<class Type> class Generator : public Allocator<C,Type> { };

    //! Default constructor
    To () { import_filter = false; }

    //! Get the value of the attribute
    std::string get_value (const std::string& name) const
      { if (!instance) return "N/A";
        else return find(name)->get_value(instance); }

    //! Set the value of the attribute
    void set_value (const std::string& name, const std::string& value)
      { find(name)->set_value(instance, value); }

    //! Get the number of attributes
    unsigned get_nattribute () const { return attributes.size(); }

    //! Get the name of the attribute
    std::string get_name (unsigned i) const 
      { return attributes[i]->get_name(); }

    //! Get the description of the attribute
    std::string get_description (unsigned i) const
      { return attributes[i]->get_description(); }

    //! Return a pointer to the named class attribute interface
    Attribute<C>* find (const std::string& name, bool ex = true) const;

    //! Set the instance
    virtual void set_instance (C* c) 
      { instance = c; }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>* parent)
      {
	for (unsigned i=0; i < parent->size(); i++)
	  if (!import_filter || !find(parent->get(i)->get_name(),false))
	    add( new IsAProxy<C,P>(parent->get(i)) );
      }

    //! Import the attribute interfaces from a member text interface
    /*! In this template, G should be of the type pointer to member function
      of C that returns pointer to M */
    template<class M, class G> 
      void import ( const std::string& name, const To<M>* member, G get )
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !find(member->get(i)->get_name(),false))
	    add(new HasAProxy<C,M,G>(name, member->get(i),get));
      }

    //! Import the attribute interfaces from a vector element text interface
    /*! In this template: G should be of the type pointer to member
      function of C that returns pointer to the indexed element of type E,
      S should be a pointer to the member function that returns the
      number of elements of type E in C. */
    template<class E, class G, class S>
      void import ( const std::string& name, const To<E>* member, G g, S s)
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !find(member->get(i)->get_name(),false))
	    add(new VectorOfProxy<C,E,G,S>(name, member->get(i), g, s));
      }

    //! Import the attribute interfaces from a map data text interface
    /*! In this template: G should be of the type pointer to member
      function of C that accepts key K and returns pointer to data element E. 
    */
    template<class K, class E, class G>
      void import ( const std::string& name, K k, const To<E>* member, G g)
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !find(member->get(i)->get_name(),false))
	    add( new MapOfProxy<C,K,E,G>(name, member->get(i), g) );
      }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>& parent)
      { import (&parent); }

    //! Import the attribute interfaces from a member text interface
    template<class M, class G> 
      void import ( const std::string& name, const To<M>& member, G get )
      { import (name, &member, get); }

    template<class E, class G, class S>
      void import ( const std::string& name, const To<E>& element, G g, S size)
      { import (name, &element, g, size); }

    template<class K, class E, class G>
      void import ( const std::string& name, K k, const To<E>& element, G g)
      { import (name, k, &element, g); }

    //! Return the number of attributes
    unsigned size () const { return attributes.size(); }

    //! Provide access to the attributes
    const Attribute<C>* get (unsigned i) const { return attributes[i]; }

  protected:

    //! When set, import filters out duplicate attribute names
    bool import_filter;

    //! Factory generates a new AttributeGet instance
    template<class P, typename T> 
      void add (T(P::*get)()const,
		const char* name, const char* description = 0)
      {
	Generator<T> gen;
	Attribute<C>* getset = gen.named (name, get);
	if (description)
	  getset->set_description (description);
	add (getset);
      }

    //! Factory generates a new AttributeGetSet instance with description
    template<class P, typename T>
      void add (T(P::*get)()const, void(P::*set)(const T&),
		const char* name, const char* description = 0)
      {
	Generator<T> gen;
	Attribute<C>* getset = gen.named (name, get, set);
	if (description)
	  getset->set_description (description);
	add (getset);
      }

    //! Factory generates a new AttributeGetSet instance with description
    template<class P, typename T>
      void add (T(P::*get)()const, void(P::*set)(T),
		const char* name, const char* description = 0)
      {
	Generator<T> gen;
	Attribute<C>* getset = gen.named (name, get, set);
	if (description)
	  getset->set_description (description);
	add (getset);
      }


    //! Add adaptable unary function object template
    template<class U>
      void add (U get, const char* name, const char* description = 0)
      {
	Attribute<C>* fget = new UnaryGet<C,U> (name, get);
	if (description)
	  fget->set_description (description);
	add (fget);
      }

    //! Add a new attribute interface
    void add (Attribute<C>* att) { attributes.push_back (att); }

    //! Remove the named attribute interface
    void remove (const std::string& name) { delete find (name); clean (); }

    //! Clean up invalid references in attributes vector
    void clean () 
      {
	unsigned i=0; 
	while ( i < attributes.size() )
	  if (!attributes[i])
	    attributes.erase( attributes.begin() + i );
	  else 
	    i++;
      }

    //! The named class attribute interfaces
    std::vector< Reference::To< Attribute<C> > > attributes;

    //! The instance of the class with which this interfaces
    Reference::To<C> instance;

  };

  //! Parse a range of indeces from a string
  void parse_indeces (std::vector<unsigned>& indeces, const std::string&);

  //! Does the work for VectorOfProxy::matches and MapOfProxy::matches
  bool match (const std::string& name, const std::string& text,
	      std::string* range, std::string* remainder);

  //! Label elements in ElementGetSet<C,E>::get_value
  extern bool label_elements;

}

template<class C>
TextInterface::Attribute<C>* 
TextInterface::To<C>::find (const std::string& param, bool throw_ex) const
{
#ifdef _DEBUG
  std::cerr << "To::find (" << param << ") size=" 
	    << attributes.size() << endl;
#endif

  for (unsigned i=0; i<attributes.size(); i++) {
    if (attributes[i]->matches (param)) {
#ifdef _DEBUG
      std::cerr << "To::find attribute[" << i << "]=" 
		<< attributes[i]->get_name() << " matches" << std::endl;
#endif
      return attributes[i];
    }
  }

  if (throw_ex)
    throw Error (InvalidParam, "TextInterface::Class<C>::find",
		 "no attribute named " + param);

  return 0;
}


template<class V, class E, class G, class S> 
std::string
TextInterface::VectorOfProxy<V,E,G,S>::get_value (const V* ptr) const
{
  std::vector<unsigned> ind;
  get_indeces (ptr, ind, range);
  std::ostringstream ost;

  for (unsigned i=0; i<ind.size(); i++) {
    if (i)
      ost << ",";  // place a comma between elements
    if (label_elements && ind.size() > 1)
      ost << ind[i] << ")";  // label the elements

    E* element = (const_cast<V*>(ptr)->*get)(ind[i]);
#ifdef _DEBUG
  std::cerr << "ElementGetSet[" << name << "]::get (" 
	    << sub_name << ") element=" << element << std::endl;
#endif
    ost << attribute->get_value (element);
  }

  return ost.str();
}

template<class V, class E, class G, class S>
void TextInterface::VectorOfProxy<V,E,G,S>::set_value (V* ptr,
						       const std::string& val)
{
  std::vector<unsigned> ind;
  get_indeces (ptr, ind, range);

  for (unsigned i=0; i<ind.size(); i++) {
    E* element = (ptr->*get)(ind[i]);
#ifdef _DEBUG
    std::cerr << "ElementGetSet[" << name << "]::set " 
	      << sub_name << "=" << value << std::endl;
#endif
    attribute->set_value (element, val);
  }
}

template<class V, class E, class G, class S>
void TextInterface::VectorOfProxy<V,E,G,S>
  ::get_indeces (const V* ptr, std::vector<unsigned>& indeces,
		 const std::string& param) const
{
#ifdef _DEBUG
  std::cerr << "ElementGetSet::get_indeces " << param << std::endl;
#endif

  std::string sub_name = param;

  if (sub_name == "*")
    return;

  parse_indeces (indeces, sub_name);

#ifdef _DEBUG
  std::cerr << "ElementGetSet::get_indeces size=" << indeces.size()
    << " name=" << sub_name << std::endl;
#endif

  unsigned num = (ptr->*size)();

  if (indeces.size() == 0) {
#ifdef _DEBUG
    std::cerr << "ElementGetSet::get_indeces select all" << std::endl;
#endif
    indeces.resize (num);
    for (unsigned i=0; i<num; i++)
      indeces[i] = i;
  }
  else {
#ifdef _DEBUG
    std::cerr << "ElementGetSet::get_indeces select";
#endif
    for (unsigned i=0; i < indeces.size(); i++) {
#ifdef _DEBUG
      std::cerr << " " << indeces[i];
#endif
      if (indeces[i] >= num)
	throw Error (InvalidRange, "TextInterface::ElementGetSet::get_indeces",
		     "%d >= %d", indeces[i], num);
    }
#ifdef _DEBUG
    std::cerr << endl;
#endif
  }
}

template<class C,class M,class Get,class Size>
 bool TextInterface::VectorOfProxy<C,M,Get,Size>::matches
  (const std::string& name) const
{
  std::string remainder;
  if (!match (prefix, name, const_cast<std::string*>(&range), &remainder))
    return false;

  return attribute->matches (remainder);
}



template<class M, class K, class E, class G> 
std::string
TextInterface::MapOfProxy<M,K,E,G>::get_value (const M* ptr) const
{
  std::vector<K> ind;
  get_indeces (ind, range);
  std::ostringstream ost;

  for (unsigned i=0; i<ind.size(); i++) {
    // place a comma between elements
    if (i)
      ost << ",";
    // label the elements
    if (label_elements && ind.size() > 1)
      ost << ind[i] << ")";

    E* element = (const_cast<M*>(ptr)->*get) (ind[i]);
    if (element)
      ost << attribute->get_value (element);
  }

  return ost.str();
}

template<class M, class K, class E, class G>
void TextInterface::MapOfProxy<M,K,E,G>::set_value (M* ptr,
						    const std::string& val)
{
  std::vector<K> ind;
  get_indeces (ind, range);

  for (unsigned i=0; i<ind.size(); i++)
    attribute->set_value ((ptr->*get)(ind[i]), val);
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
  std::string remainder;
  if (!match (prefix, name, const_cast<std::string*>(&range), &remainder))
    return false;

  return attribute->matches (remainder);
}

template<class C>
bool TextInterface::Attribute<C>::matches (const std::string& name) const
{
  return name == get_name();
}

template<class C,class M,class Get>
bool TextInterface::HasAProxy<C,M,Get>::matches (const std::string& name) const
{
  std::string::size_type length = prefix.length();

  if ( name.compare (0,length,prefix) != 0 )
    return false;

  if ( name[length] != ':' )
    return false;

  return attribute->matches (name.substr(length+1));
}


#endif
