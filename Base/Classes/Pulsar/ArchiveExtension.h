//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ArchiveExtension.h,v $
   $Revision: 1.1 $
   $Date: 2008/04/21 06:19:47 $
   $Author: straten $ */

#ifndef __Pulsar_Archive_Extension_h
#define __Pulsar_Archive_Extension_h

#include "Archive.h"

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
    
  protected:
    
    //! Extension name - useful when debugging
    std::string extension_name;
    
  };

  /*! e.g. MyExtension* ext = archive->get<MyExtension>(); */
  template<class ExtensionType>
  const ExtensionType* Archive::get () const
  {
    const ExtensionType* extension = 0;

    for (unsigned iext=0; iext<get_nextension(); iext++)
    {
      const Extension* ext = get_extension (iext);

      if (verbose == 3)
	std::cerr << "Pulsar::Archive::get<Ext> name="
		  << ext->get_extension_name() << std::endl;
      
      extension = dynamic_cast<const ExtensionType*>( ext );
      
      if (extension)
	return extension;
    }

    if (verbose==3)
      std::cerr << "Pulsar::Archive::get<Ext> failed to find extension type="
		<< typeid(extension).name() << std::endl;

    return extension;
  }

  template<class ExtensionType>
  ExtensionType* Archive::get ()
  {
    const Archive* thiz = this;
    return const_cast<ExtensionType*>( thiz->get<ExtensionType>() );
  }

  /*! If the specified ExtensionType does not exist, an atempt is made to
      add it using add_extension.  If this fails, NULL is returned. */
  template<class ExtensionType>
  ExtensionType* Archive::getadd ()
  {
    const Archive* thiz = this;
    ExtensionType* retv = 0;
    retv = const_cast<ExtensionType*>( thiz->get<ExtensionType>() );

    if (retv)
      return retv;

    try
    {
      Reference::To<ExtensionType> add_ext = new ExtensionType;
      add_extension (add_ext);
      return add_ext;
    }
    catch (Error& error)
    {
      return retv;
    }

  }

}

#endif
