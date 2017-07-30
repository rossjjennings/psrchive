//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/Registry.h

#ifndef __Registry_h
#define __Registry_h

#include "ReferenceTo.h"

#include <vector>
#include <iostream>

namespace Registry {

  template<class Parent> class List;

  //! Pure virtual template base class of Registry::List<Parent>::Enter<Child>
  template<class Parent>
  class Entry : public Reference::Able
  {
    public:

    friend class List<Parent>;

    //! Return the pointer to the instance
    Parent* get () const { return instance; }

    //! Null constructor
    Entry () { instance = 0; }

    //! Destructor deletes instance of Parent
    virtual ~Entry () { if (instance) delete instance; }
    
  protected:

    //! Adds this instance to the Parent::Registry::List<Parent>
    void register_child (Parent* _instance) 
    {
      List<Parent>& registry = List<Parent>::get_registry();

      if (List<Parent>::verbose)
        std::cerr << "Registry::Entry<Parent>::register_child"
	  " instance=" << _instance << " registry=" << &registry <<
	  " size=" << registry.size() + 1 << std::endl;

      instance = _instance;
      registry.add (this);
    }

    //! Return a pointer to a new instance of a Parent (or its children)
    virtual Parent* create () const = 0;

  private:
    Parent* instance;
  };

  //! List of Registry::Entry
  template<class Parent>
  class List : public Reference::Able
  {

    friend class Entry<Parent>;

  public:

    //! Counts the number of instances of Child entries
    template<class Child> class Instances;

    //! Enter a Child with default constructor into Registry::List<Parent>
    template<class Child> class Enter;
  
    //! Enter Child with unary constructor into Registry::List<Parent>
    template<class Child, class Argument> class Unary;

    //! Return the size of the list
    unsigned size () const { return entries.size(); }

    //! Return the specified entry
    Parent* operator[] (unsigned i) const { return entries[i]->get(); }

    //! Remove the specified entry
    void erase (unsigned i) { entries.erase( entries.begin() + i ); }

    //! Return pointer to a null-constructed instance of Parent class
    Parent* create (unsigned i) const { return entries[i]->create(); }

    //! verbosity flag used for debugging
    static bool verbose;

    //! provide access to the single registry instance
    static List& get_registry()
    { if (!registry) { auto_delete = registry = new List; } return *registry; }

  protected:

    //! Add an entry to the registry
    void add (Entry<Parent>* entry) { entries.push_back (entry); }

    //! The vector of registry entries
    std::vector< Reference::To< Entry<Parent> > > entries;

    //! The single registry instance for the Parent class
    static List* registry;

    //! Clean up the registry when the program exits
    static Reference::To<List> auto_delete;
  };

  template<class Parent>
  template<class Child>
  class List<Parent>::Instances
  {
  public:
    static unsigned get_instances () { return instances; }
    static void add_instance () { instances++; }
  private:
    static unsigned instances;
  };

  //! Enter a Child with default constructor into Registry::List<Parent>
  template<class Parent>
  template<class Child>
  class List<Parent>::Enter : public Entry<Parent>
  {
  public:
    //! Constructor registers only the first instance
    Enter () 
    { 
      if (Instances<Child>::get_instances() == 0)
	this->register_child (new Child);
      Instances<Child>::add_instance();
    }

  protected:
    //! Create a new instance of Child
    Parent* create () const { return new Child; }
  };
  
  //! Enter Child with unary constructor into Registry::List<Parent>
  template<class Parent>
  template<class Child, class Argument>
  class List<Parent>::Unary : public Entry<Parent>
  {
  public:

    //! Constructor registers only the first instance
    Unary (const Argument& arg)
    { 
      argument = arg;
      if (Instances<Child>::get_instances() == 0)
	this->register_child( new Child(arg) );
      Instances<Child>::add_instance();
    }
  protected:
    //! Create a new instance of Child
    Parent* create () const { return new Child(argument); }
    //! Argument to unary constructor
    Argument argument;
  };


}

template<class Parent> 
template<class Child>
unsigned Registry::List<Parent>::Instances<Child>::instances = 0;

template<class Parent>
bool Registry::List<Parent>::verbose = false;

template<class Parent>
Registry::List<Parent>* Registry::List<Parent>::registry = 0;

template<class Parent>
Reference::To< Registry::List<Parent> > Registry::List<Parent>::auto_delete;

#endif
