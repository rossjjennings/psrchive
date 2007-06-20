//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/WidebandCorrelator.h,v $
   $Revision: 1.8 $
   $Date: 2007/06/20 03:04:54 $
   $Author: nopeer $ */

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
    
    //! Return a text interfaces that can be used to access this instance
    Reference::To< TextInterface::Class > get_text_interface();

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

    //! Get the configuration file name
    std::string get_config() const { return configfile; }
    
    //! Get the tcycle
    double get_tcycle() const { return tcycle; }
    
    //! Get the number of receiver channels
    int get_nrcvr() const { return nrcvr; }
    
    //! Name of the configuration file used (if any)
    std::string configfile;
    
    //! Number of receiver channels
    int nrcvr;
    
    //! Fundamental correlator cycle time
    double tcycle;

  };
 

}

#endif
