//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/ArchiveExtension.h

#ifndef __Pulsar_Archive_Extension_h
#define __Pulsar_Archive_Extension_h

#include "Pulsar/Archive.h"
#include "extutil.h"

namespace Pulsar
{
  /*!
    Archive-derived classes may provide access to additional informaton
    through Extension-derived objects.
  */
  class Archive::Extension : public Reference::Able
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
    
    //! Return the name of the Extension
    std::string get_extension_name () const;
    
    //! Return an abbreviated name that can be typed relatively quickly
    virtual std::string get_short_name () const;

    //! Returns true if the Extension contains useful data
    virtual bool has_data () const { return true; }

    //! Resize the Extension
    virtual void resize (unsigned nsubint,
			 unsigned npol  = 0,
			 unsigned nchan = 0,
			 unsigned nbin  = 0) {}

    //! Derived type factory
    static Extension* factory (const std::string& name);

    //! Convenience class implements TextInterface::To<T>::get_interface_name
    template<typename T>
      class Interface : public TextInterface::To<T>
    {
      std::string get_interface_name() const
      { return this->instance->get_short_name(); }
    };

    //! Append data along frequency dimension, if applicable
    virtual void frequency_append (Archive* to, const Archive* from)
    { /* do nothing */ }
    
  protected:
    
    //! Extension name - useful when debugging
    std::string extension_name;
    
  };

  /*! e.g. 
    const Archive* archive;
    const MyExtension* ext = archive->get<MyExtension>(); */
  template<class T>
  const T* Archive::get () const
  {
    return get_ext<const T> (this, "Pulsar::Archive::get<Ext>", verbose > 2);
  }

  /*! e.g. 
    Archive* archive;
    MyExtension* ext = archive->get<MyExtension>(); */
  template<class T>
  T* Archive::get ()
  {
    return get_ext<T> (this, "Pulsar::Archive::get<Ext>", verbose > 2);
  }

  /*! If the specified extension type T does not exist, an atempt is made to
      add it using add_extension.  If this fails, NULL is returned. */
  template<class T>
  T* Archive::getadd ()
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
