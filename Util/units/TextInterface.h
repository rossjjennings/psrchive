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
    
    //! Get the name of the attribute
    virtual std::string get_name () const = 0;

    //! Get the description of the attribute
    virtual std::string get_description () const = 0;

    //! Return true if the name argument matches
    virtual bool matches (const std::string& name) const;

    //! Get the value of the attribute
    virtual std::string get_value (const C* ptr) const = 0;

    //! Set the value of the attribute
    virtual void set_value (C* ptr, const std::string& value) = 0;

  };

  //! Pointer to attribute get method, Type C::Get()
  template<class C, class Type, class Get>
  class AttributeGet : public Attribute<C> {

  public:

    //! Constructor
    AttributeGet (const std::string& _name, Get _get)
      { name = _name; get_method = _get; }

    //! Get the name of the attribute
    std::string get_name () const { return name; }

    //! Get the description of the attribute
    std::string get_description () const { return description; }

    //! Get the description of the attribute
    void set_description (const std::string& d) { description = d; }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const
      { return tostring( (ptr->*get_method) () ); }

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
    Get get_method;

  };

  //! Pointers to attribute get and set methods, Type C::Get() and C::Set(Type)
  template<class C, class Type, class Get, class Set>
  class AttributeGetSet : public AttributeGet<C, Type, Get> {

  public:

    //! Constructor
    AttributeGetSet (const std::string& _name, Get _get, Set _set)
      : AttributeGet<C,Type,Get> (_name, _get) { set_method = _set; }

    //! Set the value of the attribute
    void set_value (C* ptr, const std::string& value)
      { (ptr->*set_method) (fromstring<Type>(value)); }

  protected:

    //! The set method
    Set set_method;

  };

  //! AttributeGet and AttributeGetSet factory
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
  class ClassGetSet : public Class {

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
      {
#ifdef _DEBUG
  std::cerr << "ClassGetSet::set_instance " << c << std::endl;
#endif
	instance = c; 
      }

  protected:

    //! Add a new named class attribute interface
    void add (Attribute<C>* att) { attributes.push_back (att); }

    //! The named class attribute interfaces
    std::vector< Reference::To< Attribute<C> > > attributes;

    //! The instance of the class with which this interfaces
    Reference::To<C> instance;

  };


  //! Abstract base class of component text interface
  /*! This class is used by the CompositeGetSet class in order to
    incorporate component text interfaces. */
  template<class C>
  class Component : public Class {

  public:

    //! Get the name of the component
    virtual std::string get_component_name () const = 0;

    //! Return true if the name argument matches
    virtual bool matches (const std::string& name) const;

    //! Extract the component from the composite
    virtual void extract (C* composite) = 0;

  };

  //! Abstract base class for incorporating a component text interface
  /*! Derived classes need only define the extract_component method. */
  template<class C,class P>
  class ComponentGetSet : public Component<C> {

  public:

    //! Construct from name and component interface
    ComponentGetSet (const std::string& n, ClassGetSet<P>* part) 
      { name = n; component_interface = part; }

    //! Get the name of the component
    std::string get_component_name () const { return name; }

    std::string get_value (const std::string& n) const
      {
#ifdef _DEBUG
	std::cerr << "ComponenGetSet[" << name << "] get " << n << std::endl;
#endif
	return component_interface->get_value (n);
      }

    //! Set the value of the attribute
    void set_value (const std::string& n, const std::string& value)
      {
#ifdef _DEBUG
	std::cerr << "ComponenGetSet[" << name << "] set " << n << std::endl;
#endif
	component_interface->set_value (n, value);
      }

    unsigned get_nattribute () const
      { return component_interface->get_nattribute(); }

    std::string get_name (unsigned i) const
      { return component_interface->get_name(i); }

    std::string get_description (unsigned i) const
      { return component_interface->get_description(i); }

    //! Extract the component from the composite
    void extract (C* composite)
      {
#ifdef _DEBUG
	std::cerr << "ComponenGetSet[" << name << "] extract " << std::endl;
#endif
	component_interface->set_instance (extract_component (composite));
      }

  protected:

    //! Return a pointer to the component
    virtual P* extract_component (C*) = 0;

    //! The interface with which this interfaces
    Reference::To< ClassGetSet<P> > component_interface;

    //! The component name
    std::string name;

  };

  //! Composite text interface: a ClassGetSet with a vector of Component<C>
  /*! When a class, C, is composed of other classes that define their
   own text interface, it is useful to incorporate the component class
   text interfaces into the composite class text interface. */
  template<class C>
  class CompositeGetSet : public ClassGetSet<C> {

  public:

    //! Get the value of the attribute
    std::string get_value (const std::string& name) const;
 
    //! Set the value of the attribute
    void set_value (const std::string& name, const std::string& value);
 
    //! Get the number of attributes
    unsigned get_nattribute () const;

    //! Get the name of the attribute
    std::string get_name (unsigned i) const;

    //! Get the description of the attribute
    std::string get_description (unsigned i) const;

    //! Set the instance
    void set_instance (C* c);

    //! Import a new component interface
    void import (Component<C>* c) 
    {
      components.push_back(c);
      if (this->instance) {
#ifdef _DEBUG
	std::cerr << "CompositeGetSet::import extract " 
		  << this->instance.get() << std::endl;
#endif
	c->extract(this->instance);
      }
    }

  protected:

    //! If name is colon separated, extract component name and find it
    Component<C>* find_component (std::string& name) const;

    //! The named class attribute interfaces
    std::vector< Reference::To< Component<C> > > components;

    //! Return the index of the component for attribute index i
    unsigned get_component_index (unsigned& i) const;

  };

  //! Abstract base class for applying a text interface to multiple elements
  /*! Derived classes need only define the extract_element and get_nelement
    methods. */
  template<class C,class E>
  class ElementGetSet : public Component<C> {

  public:

    //! Construct from name and component interface
    ElementGetSet (const std::string& n, ClassGetSet<E>* part) 
      { name = n; element_interface = part; }

    //! Get the name of the component
    std::string get_component_name () const { return name + "*"; }

    //! Return true if the name argument matches
    bool matches (const std::string& name) const;

    //! Extract the component from the composite
    void extract (C* c)
      {
#ifdef _DEBUG
	std::cerr << "ElementGetSet::extract " << c << std::endl;
#endif
	composite = c;
      }

    //! Get the value(s) of the attribute
    std::string get_value (const std::string& name) const;

    //! Set the value(s) of the attribute
    void set_value (const std::string& name, const std::string& value);

    //! Get the number of attributes
    unsigned get_nattribute () const
      { return element_interface->get_nattribute(); }

    //! Get the name of the attribute
    std::string get_name (unsigned i) const
      { return element_interface->get_name(i); }

    //! Get the description of the attribute
    std::string get_description (unsigned i) const
      { return element_interface->get_description(i); }

  protected:

    //! Return a pointer to the element
    virtual E* extract_element (C*, unsigned index) const = 0;

    //! Return the number of elements
    virtual unsigned get_nelement (C*) const = 0;

    //! The interface with which this interfaces
    Reference::To< ClassGetSet<E> > element_interface;

    //! The instance from which elements should be extracted
    Reference::To< C > composite;

    //! The component name
    std::string name;

    // Helper function
    std::string get_indeces (std::vector<unsigned>& indeces, 
                             const std::string& name) const;

  };

  //! Convert a parameter name into a range of indeces
  void parse_indeces (std::vector<unsigned>& indeces, std::string& name);

  //! Label elements in ElementGetSet<C,E>::get_value
  extern bool label_elements;

}

template<class C>
TextInterface::Attribute<C>* 
TextInterface::ClassGetSet<C>::find (const std::string& param) const
{
#ifdef _DEBUG
  std::cerr << "ClassGetSet::find (" << param << ") size=" 
	    << attributes.size() << endl;
#endif

  for (unsigned i=0; i<attributes.size(); i++) {
    if (attributes[i]->matches (param)) {
#ifdef _DEBUG
      std::cerr << "ClassGetSet::find attribute[" << i << "]=" 
		<< attributes[i]->get_name() << " matches" << std::endl;
#endif
      return attributes[i];
    }
  }

  throw Error (InvalidParam, "TextInterface::Class<C>::find",
	       "no attribute named " + param);
}


template<class C>
TextInterface::Component<C>* 
TextInterface::CompositeGetSet<C>::find_component (std::string& comp) const
{
#ifdef _DEBUG
  std::cerr << "CompositeGetSet::find_component " << comp << std::endl;
#endif

  std::string cname = stringtok (comp, ":[", false, false);
  
  if (!comp.length())
    return 0;

  if (comp[0] == ':')
    comp.erase(0,1);

#ifdef _DEBUG
  std::cerr << "CompositeGetSet::find_component name=" << cname
	    << " remainder=" << comp << " size=" << components.size() 
	    << std::endl;
#endif

  for (unsigned i=0; i<components.size(); i++) {
    if (components[i]->matches(cname)) {
#ifdef _DEBUG
      std::cerr << "CompositeGetSet::find_component  component[" << i << "]=" 
		<< components[i]->get_component_name() <<  " matches"
		<< std::endl;
#endif
      return components[i];
    }
  }

  throw Error (InvalidParam, "TextInterface::Class<C>::find_component",
	       "no component named " + cname);
}

template<class C>
std::string 
TextInterface::CompositeGetSet<C>::get_value (const std::string& param) const
{
  std::string temp = param;
  Component<C>* component = find_component (temp);
  if (component)
    return component->get_value (temp);
  else
    return ClassGetSet<C>::get_value (param);
}

template<class C>
void TextInterface::CompositeGetSet<C>::set_value (const std::string& param,
						   const std::string& value)
{
  std::string temp = param;
  Component<C>* component = find_component (temp);

  if (component)
    component->set_value (temp, value);
  else
    ClassGetSet<C>::set_value (param, value);
}

template<class C>
unsigned TextInterface::CompositeGetSet<C>::get_nattribute () const
{
  unsigned nattribute = ClassGetSet<C>::get_nattribute ();
  for (unsigned i=0; i<components.size(); i++)
    nattribute += components[i]->get_nattribute ();
  return nattribute;
}

template<class C>
std::string 
TextInterface::CompositeGetSet<C>::get_name (unsigned i) const
{
  if (i < ClassGetSet<C>::get_nattribute ())
    return ClassGetSet<C>::get_name(i);

  i -= ClassGetSet<C>::get_nattribute ();
  unsigned j = get_component_index (i);
  return components[j]->get_component_name() +":"+ components[j]->get_name(i);
}

template<class C>
std::string 
TextInterface::CompositeGetSet<C>::get_description (unsigned i) const
{
  if (i < ClassGetSet<C>::get_nattribute ())
    return ClassGetSet<C>::get_description(i);

  i -= ClassGetSet<C>::get_nattribute ();
  return components[get_component_index (i)]->get_description(i);
}

template<class C>
unsigned
TextInterface::CompositeGetSet<C>::get_component_index (unsigned& i) const
{
  unsigned index = 0;
  while (i >= components[index]->get_nattribute()) {
    i -= components[index]->get_nattribute();
    index ++;
  }
  return index;
}

template<class C>
void TextInterface::CompositeGetSet<C>::set_instance (C* c)
{
#ifdef _DEBUG
  std::cerr << "CompositeGetSet::set_instance " << c 
	    << " size=" << components.size() << std::endl;
#endif

  ClassGetSet<C>::set_instance(c);

  for (unsigned i=0; i<components.size(); i++)
    components[i]->extract (c);
}

template<class C, class E>
std::string
TextInterface::ElementGetSet<C,E>::get_value (const std::string& param) const
{
  std::vector<unsigned> ind;
  std::string sub_name = get_indeces (ind, param);
  std::ostringstream ost;

  for (unsigned i=0; i<ind.size(); i++) {
    if (i)
      ost << ",";  // place a comma between elements
    if (label_elements && ind.size() > 1)
      ost << ind[i] << ")";  // label the elements

    E* element = extract_element (this->composite, ind[i]);
#ifdef _DEBUG
  std::cerr << "ElementGetSet[" << name << "]::get (" 
	    << sub_name << ") element=" << element << std::endl;
#endif
    element_interface->set_instance (element);
    ost << element_interface->get_value (sub_name);
  }

  return ost.str();
}

template<class C, class E>
void TextInterface::ElementGetSet<C,E>::set_value (const std::string& param,
						   const std::string& value)
{
  std::vector<unsigned> ind;
  std::string sub_name = get_indeces (ind, param);

  for (unsigned i=0; i<ind.size(); i++) {
    E* element = extract_element (this->composite, ind[i]);
#ifdef _DEBUG
  std::cerr << "ElementGetSet[" << name << "]::set " 
	    << sub_name << "=" << value << std::endl;
#endif
    element_interface->set_instance (element);
    element_interface->set_value (sub_name, value);
  }
}

template<class C, class E>
std::string
TextInterface::ElementGetSet<C,E>::get_indeces (std::vector<unsigned>& indeces,
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

  unsigned num = this->get_nelement(this->composite);

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

template<class C>
bool TextInterface::Component<C>::matches (const std::string& name) const
{
  return strcasecmp(name.c_str(), get_component_name().c_str()) == 0;
}

template<class C,class E>
bool TextInterface::ElementGetSet<C,E>::matches (const std::string& n) const
{
  return strncasecmp(n.c_str(), name.c_str(), name.length()) == 0;
}

#endif
