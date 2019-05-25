//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/FscrunchInterpreter.h

#ifndef __Pulsar_FscrunchInterpreter_h
#define __Pulsar_FscrunchInterpreter_h

#include <string>

namespace Pulsar {

  //! Manages Containter frequency integration for an Interpreter using an Engine
  template <class Engine>
  class FscrunchInterpreter
  {
  public:

    //! Default constructor

    template <class Container, class Interpreter>
    void fscrunch (Interpreter* interpreter, Container* container, const std::string& args)
    {
      bool scrunch_by = false;
      std::string temp = args;

      if (args[0] == 'x')
      {
        scrunch_by = true;
        temp.erase (0,1);
      }

      unsigned scrunch = interpreter->template setup<unsigned> (temp, 0);

      if (!scrunch)
        container -> fscrunch();
      else if (scrunch_by)
        container -> fscrunch (scrunch);
      else
        container -> fscrunch_to_nchan (scrunch);
    }

  protected:

    Reference::To<Engine> engine;

  };

}

#endif

