//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/StandardFeature.h,v $
   $Revision: 1.1 $
   $Date: 2008/01/17 21:11:59 $
   $Author: straten $ */

#ifndef __Pulsar_StandardFeature_h
#define __Pulsar_StandardFeature_h

#include "Pulsar/Application.h"

namespace Pulsar {

  class Interpreter;

  //! Features that employ the standard interpreter
  class StandardFeature : public Application::Feature
  {
  public:

    //! Default constructor
    StandardFeature ();

    //! Extra usage information implemented by derived classes
    virtual std::string get_usage ();

    //! Extra getopt options
    virtual std::string get_options ();

    //! Parse a non-standard command
    virtual bool parse (char code, const std::string& arg);

    //! Preprocessing tasks implemented by partially derived classes
    virtual void process (Archive*);

  private:

    // Preprocessing jobs
    std::vector<std::string> jobs;

    // standard interpreter
    Reference::To<Interpreter> interpreter;


  };

}

#endif
