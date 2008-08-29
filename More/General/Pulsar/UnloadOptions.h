//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/UnloadOptions.h,v $
   $Revision: 1.1 $
   $Date: 2008/08/29 07:00:14 $
   $Author: straten $ */

#ifndef __Pulsar_UnloadOptions_h
#define __Pulsar_UnloadOptions_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Unload interpreter command line options
  class UnloadOptions : public Application::Options
  {
  public:

    //! Default constructor
    UnloadOptions ();

    //! Additional usage information implemented by derived classes
    virtual std::string get_usage ();

    //! Additional getopt options
    virtual std::string get_options ();

    //! Parse a non-unload command
    virtual bool parse (char code, const std::string& arg);

    //! Verify setup
    virtual void setup ();

    //! Unload the archive
    virtual void finish (Archive*);

  private:

    //! Overwrite files
    bool overwrite;

    //! Write files with new extension
    std::string extension;

    //! Write files to new directory
    std::string directory;

  };

}

#endif
