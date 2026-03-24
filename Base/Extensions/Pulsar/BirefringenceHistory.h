//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/BirefringenceHistory.h

#ifndef __Pulsar_BirefringenceHistory_h
#define __Pulsar_BirefringenceHistory_h

#include "Pulsar/ColdPlasmaHistory.h"

namespace Pulsar {
  
  //! Stores parameters used to correct Faraday rotation in each Integration
  class BirefringenceHistory : public ColdPlasmaHistory
  {
  public:
    
    //! Default constructor
    BirefringenceHistory ();

    //! Copy constructor
    BirefringenceHistory (const BirefringenceHistory& extension);

    //! Assignment operator
    const BirefringenceHistory& operator= (const BirefringenceHistory& extension);
    
    //! Clone method
    BirefringenceHistory* clone () const { return new BirefringenceHistory( *this ); }
  };
  
}

#endif

