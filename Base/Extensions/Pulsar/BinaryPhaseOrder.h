//-*-C++-*-
                                                                                
/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/BinaryPhaseOrder.h,v $
   $Revision: 1.1 $
   $Date: 2003/12/07 06:35:49 $
   $Author: ahotan $ */
                                                                                
#ifndef __BinaryPhaseOrder_h
#define __BinaryPhaseOrder_h
                                                                                
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
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
    
    void organise (Archive*);
    void append   (Archive*, const Archive*);    
    void combine  (Archive*, unsigned);

  };                                                                            
  
}
                                                                                
#endif
