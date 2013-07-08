//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2003-2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/TextInterfaceAttribute.h

#ifndef __TextInterfaceAttribute_h
#define __TextInterfaceAttribute_h

#include "TextInterfaceParser.h"

namespace TextInterface
{

  //! Text interface to a class attribute
  template<class C>
  class Attribute : public Value
  {

  public:

    //! Default constructor
    Attribute ()
      { instance = 0; }

    //! Get the value of the attribute
    std::string get_value () const
      { if (!instance) return "N/A"; else return get_value (instance); }

    //! Set the value of the attribute
    void set_value (const std::string& value)
      { if (instance) set_value (instance, value); }

    //! Retun a newly constructed copy
    virtual Attribute* clone () const = 0;

    //! Get the value of the attribute
    virtual std::string get_value (const C*) const = 0;

    //! Set the value of the attribute
    virtual void set_value (C*, const std::string& value) = 0;

    //! Set the description of the value
    virtual void set_description (const std::string&) = 0;

    //! Set the detailed description of the value
    virtual void set_detailed_description (const std::string&) = 0;

     //! Pointer to the instance from which attribute value will be obtained
    mutable C* instance;

  };

  //! Interface to a class attribute with an accessor method, C::Get()
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

    //! Get the detailed description of the attribute
    std::string get_detailed_description () const
    { return detailed_description; }

    //! Get the detailed description of the attribute
    void set_detailed_description (const std::string& d)
    { detailed_description = d; }

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

    //! The detailed description of the attribute
    std::string detailed_description;

    //! The get method
    Get get;

  };

  //! Interface to a class attribute with an accessor and modifier methods
  template<class C, class Type, class Get, class Set>
  class AttributeGetSet : public AttributeGet<C, Get>
  {

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

  //! AttributeGet and AttributeGetSet factory
  /*! Use this generator function object whenever the get and set
    methods to not match the patterns supplied by the add template
    methods of the TextInterface::To class */
  template<class C, class Type>
  class Allocator {

  public:

    //! Generate a new AttributeGet instance
    template<class Get>
      AttributeGet<C,Get>* 
      operator () (const std::string& n, Get g)
      { return new AttributeGet<C,Get> (n, g); }
    
    //! Generate a new AttributeGetSet instance
    template<class Get, class Set>
      AttributeGetSet<C,Type,Get,Set>* 
      operator () (const std::string& n, Get g, Set s)
      { return new AttributeGetSet<C,Type,Get,Set> (n, g, s); }
    
    //! Generate a new AttributeGet instance with description
    template<class Get>
      AttributeGet<C,Get>* 
      operator () (const std::string& n, const std::string& d, Get g)
      {
	AttributeGet<C,Get>* get = operator () (n,g);
	get->set_description (d); return get;
      }

    //! Generate a new AttributeGetSet instance with description
    template<class Get, class Set>
      AttributeGetSet<C,Type,Get,Set>* 
      operator () (const std::string& n, const std::string& d, Get g, Set s)
      {
	AttributeGetSet<C,Type,Get,Set>* get = operator () (n,g,s);
	get->set_description (d); return get;
      }

  };

  //! Parse a range of indeces from a string
  void parse_indeces (std::vector<unsigned>& indeces, const std::string&,
		      unsigned size);

  //! Does the work for VectorOfProxy::matches and MapOfProxy::matches
  bool match (const std::string& name, const std::string& text,
	      std::string* range, std::string* remainder = 0);

}

#endif
