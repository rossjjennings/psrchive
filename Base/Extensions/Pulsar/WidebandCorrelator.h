//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/WidebandCorrelator.h,v $
   $Revision: 1.1 $
   $Date: 2004/07/12 07:34:36 $
   $Author: straten $ */

#ifndef __Pulsar_WideBandCorrelator_h
#define __Pulsar_WideBandCorrelator_h

#include "Pulsar/Backend.h"

namespace Pulsar {
  
  //! Wideband Correlator Extension
  /*! The WidebandCorrelator class stores information specific to the
    Wideband Correlator at Parkes */
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
    string get_name () const { return name; }

    //! Set the name of the backend
    void set_name (const string& _name) { name = _name; }

    //////////////////////////////////////////////////////////////////////

    //! The name of the backend
    string name;

    //! Name of the configuration file used (if any)
    string configfile;
    
    //! Number of receiver channels
    int nrcvr;
    
    //! Fundamental correlator cycle time
    double tcycle;

  };
 

}

#endif
