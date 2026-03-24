//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/TextInterfaceTransform.h

#ifndef __TextInterfaceTransform_h
#define __TextInterfaceTransform_h

#include "TextInterfaceAttribute.h"

namespace TextInterface
{

  //! Interface to a class attribute with an accessor method, C::Get()
  template<class C, class Get, class X>
  class AttributeXformGet : public AttributeGet<C,Get>
  {

  public:

    //! Constructor
    AttributeXformGet (const std::string& _name, Get _get, X _xform)
     : AttributeGet<C,Get> (_name, _get)
      { xform = _xform; }

    //! Return a clone
    Attribute<C>* clone () const { return new AttributeXformGet(*this); }

    //! Get the value of the attribute
    std::string get_value (const C* ptr) const
    { return tostring( xform( (ptr->*(this->get)) () ) ); }

  protected:
    X xform;
  };

  //! AttributeXformGet factory
  /*! Use this generator function object whenever the get and set
    methods to not match the patterns supplied by the add template
    methods of the TextInterface::To class */
  template<class C>
  class XAllocator
  {
  public:

    //! Generate a new AttributeGet instance
    template<class Get, class X>
      AttributeXformGet<C,Get,X>* 
      operator () (const std::string& n, Get g, X x)
      { return new AttributeXformGet<C,Get,X> (n, g, x); }
    
    //! Generate a new AttributeGet instance with description
    template<class Get, class X>
      AttributeXformGet<C,Get,X>* 
      operator () (const std::string& n, const std::string& d, Get g, X x)
      {
	AttributeXformGet<C,Get,X>* get = operator () (n,g,x);
	get->set_description (d); return get;
      }
  };

}

#endif
