//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver_Native.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __Receiver_Native_h
#define __Receiver_Native_h

#include "Pulsar/Receiver.h"

namespace Pulsar {

  //! Stores receptor basis, hand, and orientation, and calibrator phase
  class Receiver::Native : public Receiver::State {

  public:

    //! Default constructor
    Native ();

    //! Get the basis of the feed receptors
    Signal::Basis get_basis () const
    { return basis; }

    //! Set the basis of the feed receptors
    void set_basis (Signal::Basis _basis)
    { basis = _basis; }
    
    //! Get the orientation of the basis about the line of sight
    Angle get_orientation () const
    { return orientation; }

    //! Set the orientation of the basis about the line of sight
    void set_orientation (const Angle& angle)
    { orientation = angle; }

    //! Return true if the basis is right-handed
    bool get_right_handed () const
    { return right_handed; }

    //! Set true if the basis is right-handed
    void set_right_handed (bool right = true)
    { right_handed = right; }
    
    //! Get the phase of the reference source
    Angle get_reference_source_phase () const
    { return reference_source_phase; }

    //! Set the phase of the reference source
    void set_reference_source_phase (const Angle& phase)
    { reference_source_phase = phase; }
    
    //! Copy the state from another
    void copy (const State*);
    
  protected:
    
    //! Basis of the feed receptors
    Signal::Basis basis;
    
    //! Set true if the basis forms a right-handed coordinate system
    bool right_handed;
    
    //! The orientation of the basis about the line of sight
    Angle orientation;
    
    //! Phase of p^* q for reference source
    Angle reference_source_phase;

  };

}

#endif
