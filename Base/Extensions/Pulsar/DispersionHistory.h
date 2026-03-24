//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/DispersionHistory.h

#ifndef __Pulsar_DispersionHistory_h
#define __Pulsar_DispersionHistory_h

#include "Pulsar/ColdPlasmaHistory.h"

namespace Pulsar {
  
  //! Stores parameters used to correct dispersion in each Integration
  class DispersionHistory : public ColdPlasmaHistory {
    
  public:
    
    //! Default constructor
    DispersionHistory ();

    //! Copy constructor
    DispersionHistory (const DispersionHistory& extension);

    //! Assignment operator
    const DispersionHistory& operator= (const DispersionHistory& extension);
    
    //! Clone method
    DispersionHistory* clone () const { return new DispersionHistory( *this ); }
  };
  
}

#endif

