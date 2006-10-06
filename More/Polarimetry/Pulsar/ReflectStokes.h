//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReflectStokes.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
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
