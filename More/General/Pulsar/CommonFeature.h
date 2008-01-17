//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/CommonFeature.h,v $
   $Revision: 1.1 $
   $Date: 2008/01/17 21:50:25 $
   $Author: straten $ */

#ifndef __Pulsar_CommonFeature_h
#define __Pulsar_CommonFeature_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Features that employ the standard interpreter
  class CommonFeature : public Application::Feature
  {
  public:

    //! Default constructor
    CommonFeature ();

    //! Extra usage information implemented by derived classes
    virtual std::string get_usage ();

    //! Extra getopt options
    virtual std::string get_options ();

    //! Parse a non-standard command
    virtual bool parse (char code, const std::string& arg);

    //! Preprocessing tasks implemented by partially derived classes
    virtual void process (Archive*);

  private:

    bool tscrunch, fscrunch, pscrunch;

  };

}

#endif
