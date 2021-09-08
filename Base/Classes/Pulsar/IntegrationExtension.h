//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/IntegrationExtension.h

#ifndef __Pulsar_Integration_Extension_h
#define __Pulsar_Integration_Extension_h

#include "Pulsar/Integration.h"
#include "TextInterface.h"
#include "extutil.h"

namespace Pulsar
{
  /*!
    Integration-derived classes may provide access to additional informaton
    through Extension-derived objects.
  */
  class Integration::Extension : public Reference::Able
  {
  public:

    //! Construct with a name
    Extension (const char* name);

    //! Destructor
    virtual ~Extension ();
    
    //! Return a new copy-constructed instance identical to this instance
    virtual Extension* clone () const = 0;
    
    //! Return a text interface that can be used to access this instance
    virtual TextInterface::Parser* get_interface () { return 0; }
    
    //! Integrate information from another Integration
    virtual void integrate (const Integration* subint) { }
      
    //! Update information based on the provided Integration
    virtual void update (const Integration* subint) { }

    //! Return the name of the Extension
    std::string get_extension_name () const;
    
    //! Return an abbreviated name that can be typed relatively quickly
    virtual std::string get_short_name () const;

    //! Derived type factory
    static Extension* factory (const std::string& name);

    //! Convenience class implements TextInterface::To<T>::get_interface_name
    template<typename T>
    class Interface : public TextInterface::To<T>
    {
      std::string get_interface_name() const
      { return this->instance->get_short_name(); }
    };
    
  protected:
    
    //! Extension name - useful when debugging
    std::string extension_name;
    
    //! Provide Extension derived classes with access to parent Archive
    const Archive* get_parent (const Integration* subint) const;
  };

  /*! e.g. 
    const Integration* integration;
    const MyExtension* ext = integration->get<MyExtension>(); */
  template<class T>
  const T* Integration::get () const
  {
    return get_ext<const T> (this, "Pulsar::Integration::get<Ext>", verbose);
  }

  /*! e.g. 
    Integration* integration;
    MyExtension* ext = integration->get<MyExtension>(); */
  template<class T>
  T* Integration::get ()
  {
    return get_ext<T> (this, "Pulsar::Integration::get<Ext>", verbose);
  }

  /*! If the specified extension type T does not exist, an atempt is made to
      add it using add_extension. */
  template<class T>
  T* Integration::getadd ()
  {
    T* ext = get<T>();
    if (ext)
      return ext;

    Reference::To<T> add_ext = new T;
    add_extension (add_ext);
    return add_ext;
  }
}

#endif
