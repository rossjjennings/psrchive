//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Arecibo_h
#define __Arecibo_h

namespace Pulsar {

  class Archive;
  class Receiver;

  //! Namespace contains GBT receiver info
  namespace Arecibo {

    //! Initialize the Receiver Extension L-wide
    void Lwide(Receiver *r);

    //! Initialize the Receiver Extension with Arecibo best guess
    void guess (Receiver* r, Archive* a);

  } 

}

#endif
