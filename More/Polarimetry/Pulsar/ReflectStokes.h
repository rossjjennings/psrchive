//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReflectStokes.h,v $
   $Revision: 1.1 $
   $Date: 2005/06/08 04:33:36 $
   $Author: straten $ */

#ifndef __Pulsar_ReflectStokes_h
#define __Pulsar_ReflectStokes_h

namespace Pulsar {

  class Archive;

  class ReflectStokes {

  public:
    //! Constructor
    ReflectStokes ();

    //! Perform reflections on Pulsar Archive
    void operate (Archive*);

    //! Add a reflection; stokes = 'q', 'u', or 'v'
    void add_reflection (char stokes);

  protected:
    //! The reflections
    char reflect;

  };

}

#endif
