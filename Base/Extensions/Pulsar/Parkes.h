//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Parkes.h,v $
   $Revision: 1.2 $
   $Date: 2004/08/09 01:12:57 $
   $Author: sord $ */

#ifndef __Parkes_h
#define __Parkes_h

namespace Pulsar {

  class Archive;
  class Receiver;

  //! Namespace contains Parkes-specific routines
  namespace Parkes {

    //! Initialize the Receiver Extension with Parkes Multibeam attributes
    void Multibeam (Receiver* receiver);

    //! Initialize the Receiver Extension with Parkes H_OH attributes
    void H_OH (Receiver* receiver);

    //! Initialize the Receiver Extension with Parkes H_OH attributes
    void COAX10_50 (Receiver* receiver);

    //! Initialize the Receiver Extension with Parkes best guess
    void guess (Receiver* receiver, Archive* archive);

  } 

}

#endif
