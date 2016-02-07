//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/ObsDescription.h

#ifndef __ObsDescription_h
#define __ObsDescription_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {

  //! Stores pulsar parameters as uninterpreted text
  class ObsDescription : public Archive::Extension {

  public:

    //! Default constructor
    ObsDescription ();

    //! Copy constructor
    ObsDescription (const ObsDescription&);

    //! Clone method
    ObsDescription* clone () const;

    //! Load from an open stream
    void load (FILE*);

    //! Unload to an open stream
    void unload (FILE*) const;

    //! Set the text
    void set_text (const std::string& t) { text = t; }

    //! Get the text
    std::string get_text () const { return text; }

  protected:

    std::string text;
  };

}

#endif
