//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
                                                                                
// psrchive/Base/Extensions/Pulsar/BinaryPhaseOrder.h
                                                                                
#ifndef __BinaryPhaseOrder_h
#define __BinaryPhaseOrder_h
                                                                                
#include "Pulsar/IntegrationOrder.h"
                                                                            
namespace Pulsar {
  
  /*! This BinaryPhaseOrder class provides a means for developers to
    re-index the way Integrations are stored. It is a virtual base class
    that must be inherited by developers who wish to define a new way to
    index the Integrations in their Archive */
  
  class BinaryPhaseOrder : public Pulsar::IntegrationOrder {
                                                                                
  public:
    
    // These are the standard Archive::Extension methods:

    //! Default constructor
    BinaryPhaseOrder ();
    
    //! Destructor
    ~BinaryPhaseOrder ();
    
    //! Copy constructor
    BinaryPhaseOrder (const BinaryPhaseOrder& extension);
    
    //! Operator =
    const BinaryPhaseOrder& operator= (const BinaryPhaseOrder& extension);
    
    //! Clone method
    IntegrationOrder* clone () const;
    
    void organise (Archive*, unsigned);
    void append   (Archive*, const Archive*);    
    void combine  (Archive*, unsigned);

  };                                                                            
  
}
                                                                                
#endif
