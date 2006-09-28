//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/WidebandCorrelator.h,v $
   $Revision: 1.4 $
   $Date: 2006/09/28 16:24:52 $
   $Author: straten $ */

#ifndef __Pulsar_WideBandCorrelator_h
#define __Pulsar_WideBandCorrelator_h

#include "Pulsar/Backend.h"

namespace Pulsar {
  
  //! Stores Parkes Wideband Correlator parameters
  /*! This class stores information specific to the Wideband Correlator at
    Parkes */
  class WidebandCorrelator : public Pulsar::Backend {

  public:
    
    //! Default constructor
    WidebandCorrelator ();

    //! Copy constructor
    WidebandCorrelator (const WidebandCorrelator& extension);

    //! Assignment operator
    const WidebandCorrelator& operator= (const WidebandCorrelator& extension);

    //! Destructor
    ~WidebandCorrelator ();

    //////////////////////////////////////////////////////////////////////
    //
    // Archive::Extension implementation
    //
    //////////////////////////////////////////////////////////////////////

    //! Clone method
    WidebandCorrelator* clone () const
    { return new WidebandCorrelator( *this ); }

    //////////////////////////////////////////////////////////////////////
    //
    // Backend implementation
    //
    //////////////////////////////////////////////////////////////////////
    
    //! Get the name of the backend
    std::string get_name () const { return name; }

    //! Set the name of the backend
    void set_name (const std::string& _name) { name = _name; }

    //////////////////////////////////////////////////////////////////////

    //! The name of the backend
    std::string name;

    //! Name of the configuration file used (if any)
    std::string configfile;
    
    //! Number of receiver channels
    int nrcvr;
    
    //! Fundamental correlator cycle time
    double tcycle;

  };
 

}

#endif
