//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/WidebandCorrelator.h

#ifndef __Pulsar_WideBandCorrelator_h
#define __Pulsar_WideBandCorrelator_h

#include "Pulsar/Backend.h"
#include "TextInterface.h"

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
    TextInterface::Parser* get_interface();

    // Text interface to a WidebandCorrelator instance
    class Interface : public TextInterface::To<WidebandCorrelator>
    {
      public:
	Interface( WidebandCorrelator *s_instance = NULL );
    };

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
    void set_config( const std::string& config ) { configfile = config; }

    //! Get the tcycle
    double get_tcycle() const { return tcycle; }
    void set_tcycle(const double _tcycle) { tcycle = _tcycle; }
    
    //! Get the number of receiver channels
    int get_nrcvr() const { return nrcvr; }
    void set_nrcvr(int _nrcvr) { nrcvr = _nrcvr; }
    
    //! Name of the configuration file used (if any)
    std::string configfile;
    
    //! Number of receiver channels
    int nrcvr;
    
    //! Fundamental correlator cycle time
    double tcycle;

  };
 

}

#endif
