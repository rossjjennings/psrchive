#ifndef __TextInterface_h
#define __TextInterface_h

#include "Reference.h"
#include "Error.h"
#include "tostring.h"
#include "stringtok.h"

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

  class Class : public Reference::Able {

  public:

    //! Get the value of the attribute
    virtual std::string get_value (const std::string& name) const = 0;

    //! Set the value of the attribute
    virtual void set_value (const std::string& name, 
			    const std::string& value) = 0;

  };


  template<class C>
  class ClassGetSet : public Class {

  public:

    template<class Type> class Generator : public Allocator<C,Type> { };

    //! Get the value of the attribute
    virtual std::string get_value (const std::string& name) const
      { return find(name)->get_value(instance); }

    //! Set the value of the attribute
    virtual void set_value (const std::string& name, 
				const std::string& value)
      { find(name)->set_value(instance, value); }

    //! Return a pointer to the named class attribute interface
    Attribute<C>* find (const std::string& name) const;

    //! Set the instance
    virtual void set_instance (C* c) { instance = c; }

  protected:

    //! Add a new named class attribute interface
    void add (Attribute<C>* att) { attributes.push_back (att); }

    //! The named class attribute interfaces
    std::vector< Reference::To< Attribute<C> > > attributes;

    //! The instance of the class with which this interfaces
    Reference::To<C> instance;

  };


  //! Base class of component glue
  template<class Whole>
  class Component : public Class {

  public:

    //! Get the name of the component
    virtual std::string get_name () const = 0;

    //! Return true if the name argument matches
    virtual bool matches (const std::string& name) const;

    //! Extract the component from the composite
    virtual void extract (Whole* composite) = 0;

  };

  template<class Whole,class PartInterface>
  class ComponentGetSet : public Component<Whole> {

  public:

    //! Construct from name and component interface
    ComponentGetSet (const std::string& n, PartInterface* part) 
      { name = n; part_interface = part; }

    //! Get the name of the component
    std::string get_name () const { return name; }

    std::string get_value (const std::string& name) const
      { return part_interface->get_value(name); }

    //! Set the value of the attribute
    void set_value (const std::string& name, const std::string& value)
      { part_interface->set_value(name, value); }

  protected:

    //! The interface with which this interfaces
    Reference::To<PartInterface> part_interface;

    //! The component name
    std::string name;

  };

  template<class C>
  class CompositeGetSet : public ClassGetSet<C> {

  public:

    //! Get the value of the attribute
    std::string get_value (const std::string& name) const;
 
    //! Set the value of the attribute
    void set_value (const std::string& name, const std::string& value);
 
    //! Set the instance
    void set_instance (C* c);

    //! Import a new component interface
    void import (Component<C>* c) 
      { components.push_back(c);if(this->instance)c->extract(this->instance); }

  protected:

    //! If name is colon separated, extract component name and find it
    Component<C>* find_component (std::string& name) const;

    //! The named class attribute interfaces
    std::vector< Reference::To< Component<C> > > components;

  };

}

template<class C>
TextInterface::Attribute<C>* 
TextInterface::ClassGetSet<C>::find (const std::string& name) const
{ 
  for (unsigned i=0; i<attributes.size(); i++)
    if (attributes[i]->matches(name))
      return attributes[i];
  throw Error (InvalidParam, "TextInterface::Class<C>::find",
	       "no attribute named " + name);
}


template<class C>
TextInterface::Component<C>* 
TextInterface::CompositeGetSet<C>::find_component (std::string& name) const
{
  std::string temp = name;
  std::string cname = stringtok (temp, ": \t\n");
  std::string aname = stringtok (temp, " \t\n");

  if (!aname.length())
    return 0;

  name = aname;

  for (unsigned i=0; i<components.size(); i++)
    if (components[i]->matches(cname))
      return components[i];

  throw Error (InvalidParam, "TextInterface::Class<C>::find_component",
	       "no component named " + cname);
}

template<class C>
std::string 
TextInterface::CompositeGetSet<C>::get_value (const std::string& name) const
{
  std::string temp = name;
  Component<C>* component = find_component (temp);
  if (component)
    return component->get_value (temp);
  else
    return ClassGetSet<C>::get_value (name);
}

template<class C>
void TextInterface::CompositeGetSet<C>::set_value (const std::string& name,
						   const std::string& value)
{
  std::string temp = name;
  Component<C>* component = find_component (temp);

  if (component)
    component->set_value (temp, value);
  else
    ClassGetSet<C>::set_value (name, value);
}
 
template<class C>
void TextInterface::CompositeGetSet<C>::set_instance (C* c)
{
  ClassGetSet<C>::set_instance(c);
  for (unsigned i=0; i<components.size(); i++)
    components[i]->extract (c);
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
  return strcasecmp(name.c_str(), get_name().c_str()) == 0;
}

#endif
