//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2003 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/TextInterfaceTo.h

#ifndef __TextInterfaceTo_h
#define __TextInterfaceTo_h

#include "TextInterfaceAttribute.h"
#include "TextInterfaceFunction.h"
#include "TextInterfaceElement.h"
#include "TextInterfaceProxy.h"
#include "TextInterfaceEmbed.h"

namespace TextInterface
{
  //! Class text interface: an instance of C and a vector of Attribute<C>
  template<class C>
  class To : public Parser {

  public:

    template<class Type> class Generator : public Allocator<C,Type> { };

    template<class Type> class VGenerator : public VAllocator<C,Type> { };

    //! Set the instance
    virtual void set_instance (C* c) 
    {
      instance = c; 
      for (unsigned i=0; i<values.size(); i++)
	setup (values[i]);
    }

    //! Set the instance of the Attribute<C>
    void setup (const Value* value)
    {
#if _DEBUG
      std::cerr << "TextInterface::To<C>::setup"
	" name=" << value->get_name() <<
	" instance=" << (void*) instance.ptr() << std::endl;
#endif
      const Attribute<C>* attribute = dynamic_cast<const Attribute<C>*>(value);
      if (attribute) 
	attribute->instance = instance.ptr();
#if _DEBUG
      else
	std::cerr << "TextInterface::To<C>::setup " << value->get_name() <<
	  " is not an Attribute<C>" << std::endl;
#endif
    }

    void add_value( Attribute<C>* value )
    {
      if (instance)
	value->instance = instance;

      Parser::add_value( value );
    }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>* parent)
      {
	for (unsigned i=0; i < parent->size(); i++)
	  if (!import_filter || !found(parent->get(i)->get_name()))
	    add_value( new IsAProxy<C,P>(parent->get(i)) );
      }

    //! Import the attribute interfaces from a member text interface
    /*! In this template, G should be of the type pointer to member function
      of C that returns pointer to M */
    template<class M, class G> 
      void import (const std::string& name, const To<M>* member, G get)
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !found(name, member->get(i)->get_name()))
	    add_value( new HasAProxy<C,M,G>(name, member->get(i), get) );
      }

    //! Import the attribute interfaces from a vector element text interface
    /*! In this template: G should be of the type pointer to member
      function of C that returns pointer to the indexed element of type E,
      S should be a pointer to the member function that returns the
      number of elements of type E in C. */
    template<class E, class G, class S>
      void import (const std::string& name, const To<E>* member, G g, S s)
      {
	for (unsigned i=0; i < member->size(); i++)
	{
	  const Attribute<E>* value = member->get(i);
	  if (!value)
	    continue;

#ifdef _DEBUG
	  std::cerr << "TextInterface::import name="
		    << value->get_name() << std::endl;
#endif

	  if (!import_filter || !found(name, value->get_name()))
	    add_value(new VectorOfProxy<C,E,G,S>(name, value, g, s));
	}
      }

    //! Import the attribute interfaces from a map data text interface
    /*! In this template: G should be of the type pointer to member
      function of C that accepts key K and returns pointer to data element E. 
    */
    template<class K, class E, class G>
      void import (const std::string& name, K, const To<E>* member, G g)
      {
	for (unsigned i=0; i < member->size(); i++)
	  if (!import_filter || !found(name, member->get(i)->get_name()))
	    add_value( new MapOfProxy<C,K,E,G>(name, member->get(i), g) );
      }

    //! Import the attribute interfaces from a parent text interface
    template<class P> 
      void import (const To<P>& parent)
      { import (&parent); }

    //! Import the attribute interfaces from a member text interface
    template<class M, class G> 
      void import ( const std::string& name, const To<M>& member, G get )
      { import (name, &member, get); }

    //! Import the attribute interfaces from a member text interface
    template<class M, class G> 
      void import ( const To<M>& member, G get )
    { import (std::string(), &member, get); }

    template<class E, class G, class S>
      void import ( const std::string& name, const To<E>& element, G g, S size)
      { import (name, &element, g, size); }

    template<class K, class E, class G>
      void import ( const std::string& name, K k, const To<E>& element, G g)
      { import (name, k, &element, g); }

    //! Return the number of attributes
    unsigned size () const { return values.size(); }

    //! Provide access to the attributes
    const Attribute<C>* get (unsigned i) const
      { return dynamic_cast< const Attribute<C>* >( values[i].get() ); }

  protected:

    //! Factory generates a new AttributeGet instance
    template<class P, typename T> 
      void add (T(P::*get)()const,
		const char* name, const char* description = 0)
      {
	Generator<T> gen;
	Attribute<C>* getset = gen (name, get);
	if (description)
	  getset->set_description (description);
	add_value (getset);
      }

    //! Factory generates a new AttributeGetSet instance with description
    template<class P, typename T, typename U>
      void add (T(P::*get)()const, void(P::*set)(const U&),
		const char* name, const char* description = 0)
      {
	Generator<U> gen;
	Attribute<C>* getset = gen (name, get, set);
	if (description)
	  getset->set_description (description);
	add_value (getset);
      }

    //! Factory generates a new AttributeGetSet instance with description
    template<class P, typename T, typename U>
      void add (T(P::*get)()const, void(P::*set)(U),
		const char* name, const char* description = 0)
      {
	Generator<U> gen;
	Attribute<C>* getset = gen (name, get, set);
	if (description)
	  getset->set_description (description);
	add_value (getset);
      }
    
    //! Factory generates a new DirectInterface instance with description
    template<class P, typename T, typename U>
    void add (T(P::*get)()const, void(P::*set)(const U&), 
	      Parser*(P::*get_parser)(),
	      const char* name, const char* description = 0)
    {
      EmbedAllocator<C,U> gen;
      Attribute<C>* getset = gen.direct (name, get, set, get_parser);
      if (description)
	getset->set_description (description);
      add_value (getset);
    }

    //! Factory generates a new DirectInterface instance with description
    template<class P, typename T, typename U>
    void add (T(P::*get)()const, void(P::*set)(U), 
	      Parser*(P::*get_parser)(),
	      const char* name, const char* description = 0)
    {
      EmbedAllocator<C,U> gen;
      Attribute<C>* getset = gen.direct (name, get, set, get_parser);
      if (description)
	getset->set_description (description);
      add_value (getset);
    }

    //! Factory generates a new IndirectInterface instance with description
    template<class P, typename U, typename Parent>
    void add (U*(P::*get)()const, void(P::*set)(U*), 
	      Parser*(Parent::*get_parser)(),
	      const char* name, const char* description = 0)
    {
      EmbedAllocator<C,U*> gen;
      Attribute<C>* getset = gen.indirect (name, get, set, get_parser);
      if (description)
	getset->set_description (description);
      add_value (getset);
    }

    //! Factory generates a new DirectInterface instance with description
    template<class Get, class Set, class Parser>
    void add (Get get, Set set, Parser parser,
              const char* name, const char* description = 0)
    {
      EmbedAllocator<C,typename Set::second_argument_type> gen;
      Attribute<C>* getset = gen.direct (name, get, set, parser);
      if (description)
        getset->set_description (description);
      add_value (getset);
    }

    //! Add adaptable unary function object template
    template<class Get>
      void add (Get get, const char* name, const char* description = 0)
      {
	Attribute<C>* fget = new GetFunction<C,Get> (name, get);
	if (description)
	  fget->set_description (description);
	add_value (fget);
      }

    //! Add adaptable unary function object template
    template<class Get, class Set>
      void add (Get get, Set set, const char* name, const char* description = 0)
      {
        Attribute<C>* fget = new GetSetFunction<C,Get,Set> (name, get, set);
        if (description)
          fget->set_description (description);
        add_value (fget);
      }

    //! The instance of the class with which this interfaces
    Reference::To<C,false> instance;

  };

}

#endif
