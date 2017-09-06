//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
                                                                                
// psrchive/Base/Extensions/Pulsar/BinLngAscOrder.h
                                                                                
#ifndef __BinLngAscOrder_h
#define __BinLngAscOrder_h
                                                                                
#include "Pulsar/IntegrationOrder.h"
                                                                            
namespace Pulsar {
  
  /*! This BinLngAscOrder class provides a means for developers to
    re-index the way Integrations are stored. It is a virtual base class
    that must be inherited by developers who wish to define a new way to
    index the Integrations in their Archive */
  
  class BinLngAscOrder : public Pulsar::IntegrationOrder {
                                                                                
  public:
    
    // These are the standard Archive::Extension methods:

    //! Default constructor
    BinLngAscOrder ();
    
    //! Destructor
    ~BinLngAscOrder ();
    
    //! Copy constructor
    BinLngAscOrder (const BinLngAscOrder& extension);
    
    //! Operator =
    const BinLngAscOrder& operator= (const BinLngAscOrder& extension);
    
    //! Clone method
    IntegrationOrder* clone () const;
    
    void organise (Archive*, unsigned);
    void append   (Archive*, const Archive*);    
    void combine  (Archive*, unsigned);

  };                                                                            
  
}
                                                                                
#endif
