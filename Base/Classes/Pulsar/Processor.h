//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Processor.h

#ifndef __Pulsar_Processor_h
#define __Pulsar_Processor_h

#include "Reference.h"

namespace Pulsar {

  class Archive;

  //! Interface to any object that processes data in an Archive
  class Processor : public Reference::Able
  {
  public:

    //! Process the archive (either in place or out of place)
    virtual void process (Archive*) = 0;

    //! If process if out of place, return pointer to new result 
    /*! 
      By default, in place operations return a NULL pointer
    */
    virtual Archive* result () { return 0; }
  };

}

#endif
