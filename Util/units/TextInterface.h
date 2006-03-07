#ifndef __TextInterface_h
#define __TextInterface_h

#include "Reference.h"
#include "Error.h"
#include "tostring.h"
#include "stringtok.h"

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
    IsAProxy (Attribute<P>* pa) { attribute = pa->clone(); }

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
    
    //! Construct from a pointer to parent class attribute interface
    HasAProxy (const std::string& pre, Attribute<M>* pa, Get g)
      { prefix = pre; attribute = pa->clone(); get = g; }

    //! Copy constructor
    HasAProxy (const HasAProxy& copy)
      { attribute=copy.attribute->clone(); get=copy.get; prefix=copy.prefix; }

    //! Set the prefix to be added before attribute name
    
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

    //! Prefix differentiates this interface from identical members (if any)
    std::string prefix;

  };

  //! Proxy enables attribute interface of elements of a vector
  /*! In this template: V is a vector of E; Get is the method of V
    that returns E*; and Size is the method of V that returns the number
    of elements */
  template<class V, class E, class Get, class Size>
  class VectorOfProxy : public Attribute<V> {

  public:
    
    //! Construct from a pointer to parent class attribute interface
    VectorOfProxy (const std::string& pre, Attribute<E>* pa, Get g, Size s)
      { prefix = pre; attribute = pa->clone(); get = g; size = s; }

    //! Copy constructor
    VectorOfProxy (const VectorOfProxy& copy)
      { attribute=copy.attribute->clone(); 
	get=copy.get; size=copy.size; prefix=copy.prefix; }

    //! Set the prefix to be added before attribute name
    
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

    //! Prefix differentiates this interface from identical members (if any)
    std::string prefix;

    //! Range parsed from name during matches
    std::string range;

    //! Worker function parses indeces for get_value and set_value
    std::string get_indeces (const V* ptr, std::vector<unsigned>& indeces,
			     const std::string& param) const;

  };

  //! Pointer to attribute get method, Type C::Get()
  template<class C, class Type, class Get>
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
      { return tostring( (ptr->*get) () ); }

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

  //! Pointers to attribute get and set methods, Type C::Get() and C::Set(Type)
  template<class C, class Type, class Get, class Set>
  class AttributeGetSet : public AttributeGet<C, Type, Get> {

  public:

    //! Constructor
    AttributeGetSet (const std::string& _name, Get _get, Set _set)
      : AttributeGet<C,Type,Get> (_name, _get) { set = _set; }

    //! Copy constructor
    AttributeGetSet (const AttributeGetSet& copy) 
      : AttributeGet<C,Type,Get> (copy) { set = copy.set; }

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
      AttributeGet<C,Type,Get>* 
      named (const std::string& n, Get g)
      { return new AttributeGet<C,Type,Get> (n, g); }
    
    //! Generate a new AttributeGetSet instance
    template<class Get, class Set>
      AttributeGetSet<C,Type,Get,Set>* 
      named (const std::string& n, Get g, Set s)
      { return new AttributeGetSet<C,Type,Get,Set> (n, g, s); }
    
    //! Generate a new AttributeGet instance with description
    template<class Get>
      AttributeGet<C,Type,Get>* 
      described (const std::string& n, const std::string& d, Get g)
      {
	AttributeGet<C,Type,Get>* get = named (n,g);
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
    Attribute<C>* find (const std::string& name) const;

    //! Set the instance
    virtual void set_instance (C* c) 
      { instance = c; }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>* parent)
      {
	for (unsigned i=0; i < parent->attributes.size(); i++)
	  add( new IsAProxy<C,P>(parent->attributes[i]) );
      }

    //! Import the attribute interfaces from a member text interface
    template<class M> 
      void import ( const std::string& name, 
		    const To<M>* member, M*(C::*get)() )
      {
	for (unsigned i=0; i < member->attributes.size(); i++)
	  add(new HasAProxy<C,M,M*(C::*)()>(name, member->attributes[i],get));
      }

    //! Import the attribute interfaces from a vector element text interface
    template<class E>
      void import ( const std::string& name,
		    const To<E>* member, 
		    E*(C::*get)(unsigned), unsigned(C::*size)()const )
      {
	for (unsigned i=0; i < member->attributes.size(); i++)
	  add( new VectorOfProxy<C,E,E*(C::*)(unsigned),unsigned(C::*)()const>
	       (name, member->attributes[i], get, size) );
      }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>& parent)
      { import (&parent); }

    //! Import the attribute interfaces from a member text interface
    template<class M> 
      void import ( const std::string& name,
		    const To<M>& member,
		    M*(C::*get)() ) 
      { import (name, &member, get); }

    template<class E>
      void import ( const std::string& name,
		    const To<E>& element, 
		    E*(C::*get)(unsigned), unsigned(C::*size)()const )
      { import (name, &element, get, size); }

  protected:

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

    // allow different To types to see eachothers protected bit
    template<class P> friend class To;

    //! The named class attribute interfaces
    std::vector< Reference::To< Attribute<C> > > attributes;

    //! The instance of the class with which this interfaces
    Reference::To<C> instance;

  };

  //! Parse a range of indeces from a string
  void parse_indeces (std::vector<unsigned>& indeces, const std::string&);

  //! Separate a list of comma-separated commands into a vector of strings
  void separate (char* ptr, std::vector<std::string>& commands, bool& edit);
  void separate (char* ptr, std::vector<std::string>& commands);

  //! Label elements in ElementGetSet<C,E>::get_value
  extern bool label_elements;

}

template<class C>
TextInterface::Attribute<C>* 
TextInterface::To<C>::find (const std::string& param) const
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

  throw Error (InvalidParam, "TextInterface::Class<C>::find",
	       "no attribute named " + param);
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
  std::string TextInterface::VectorOfProxy<V,E,G,S>
  ::get_indeces (const V* ptr, std::vector<unsigned>& indeces,
		 const std::string& param) const
{
#ifdef _DEBUG
  std::cerr << "ElementGetSet::get_indeces " << param << std::endl;
#endif

  std::string sub_name = param;
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

  return sub_name;
}


#include <string.h>

template<class C>
bool TextInterface::Attribute<C>::matches (const std::string& name) const
{
  return strcasecmp(name.c_str(), get_name().c_str()) == 0;
}

template<class C,class M,class Get>
bool TextInterface::HasAProxy<C,M,Get>::matches (const std::string& name) const
{
  unsigned length = prefix.length();
  if ( strncasecmp(prefix.c_str(), name.c_str(), length) != 0 )
    return false;

  if ( name[length] != ':' )
    return false;

  return attribute->matches (name.substr(length+1));
}

template<class C,class M,class Get,class Size>
 bool TextInterface::VectorOfProxy<C,M,Get,Size>::matches
  (const std::string& name) const
{
  unsigned length = prefix.length();
  if ( strncasecmp(prefix.c_str(), name.c_str(), length) != 0 )
    return false;

  std::string::size_type end = name.find (':', length);
  if (end == std::string::npos)
    return false;

  // store the range string for later use in get|set_value
  *(const_cast<std::string*>(&range)) = name.substr (length, end-length);

  std::string remainder = name.substr(end+1);
  return attribute->matches (remainder);
}

#endif
