#ifndef __TextInterface_h
#define __TextInterface_h

#include "Reference.h"
#include "Error.h"
#include "tostring.h"

namespace TextInterface {

  //! Abstract base class of attribute text interface
  template<class Class>
  class Attribute : public Reference::Able {

  public:
    
    //! Get the name of the attribute
    virtual std::string get_name () const = 0;

    //! Get the description of the attribute
    virtual std::string get_description () const = 0;

    //! Return true if the name argument matches
    virtual bool matches (const std::string& name) const;

    //! Get the value of the attribute
    virtual std::string get_value (const Class* ptr) const = 0;

    //! Set the value of the attribute
    virtual void set_value (Class* ptr, const std::string& value) = 0;

  };

  template<class Class, class Type, class Get>
  class AttributeGet : public Attribute<Class> {

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
    std::string get_value (const Class* ptr) const
      { return tostring( (ptr->*get_method) () ); }

    //! Set the value of the attribute
    void set_value (Class* ptr, const std::string& value)
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

  template<class Class, class Type, class Get, class Set>
  class AttributeGetSet : public AttributeGet<Class, Type, Get> {

  public:

    //! Constructor
    AttributeGetSet (const std::string& _name, Get _get, Set _set)
      : AttributeGet<Class,Type,Get> (_name, _get) { set_method = _set; }

    //! Set the value of the attribute
    void set_value (Class* ptr, const std::string& value)
      { (ptr->*set_method) (fromstring<Type>(value)); }

  protected:

    //! The set method
    Set set_method;

  };

  template<class Class, class Type>
  class Allocator {

  public:

    template<class Get>
      AttributeGet<Class,Type,Get>* 
      new_attribute (const std::string& n, Get g)
      { return new AttributeGet<Class,Type,Get> (n, g); }

    template<class Get, class Set>
    AttributeGetSet<Class,Type,Get,Set>* 
      new_attribute (const std::string& n, Get g, Set s)
      { return new AttributeGetSet<Class,Type,Get,Set> (n, g, s); }

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

    //! Add a new named class attribute interface
    void add_attribute (Attribute<C>* att) { attributes.push_back (att); }

  protected:

    //! The named class attribute interfaces
    std::vector< Reference::To< Attribute<C> > > attributes;

    //! The instance of the class with which this interfaces
    Reference::To<C> instance;

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
	       "no attributes named " + name);
}

#include <string.h>

template<class Class>
bool TextInterface::Attribute<Class>::matches (const std::string& name) const
{
  return strcasecmp(name.c_str(), get_name().c_str()) == 0;
}


#endif
