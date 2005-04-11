//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver_Linear.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/11 10:26:15 $
   $Author: straten $ */

#ifndef __Receiver_Linear_h
#define __Receiver_Linear_h

#include "Pulsar/Receiver.h"

namespace Pulsar {

  //! Directly implements Receiver State
  class Receiver_Linear : public Receiver::State {

  public:

    //! Default constructor
    Receiver_Linear ();

    //! Get the basis of the feed receptors
    Signal::Basis get_basis () const;
    
    //! Get the orientation of the basis about the line of sight
    Angle get_orientation () const;
    
    //! Return true if the basis is right-handed
    bool get_right_handed () const;
    
    //! Get the phase of the reference source
    Angle get_reference_source_phase () const;
    
    //! Copy the state from another
    void copy (const State*);
    
    //! Get the offset of the feed X axis with respect to the platform zero
    Angle get_X_offset () const;
    //! Set the offset of the feed X axis with respect to the platform zero
    void set_X_offset (const Angle& offset);
    
    //! Get the offset of the feed Y axis from its nominal value
    Angle get_Y_offset () const;
    //! Set the offset of the feed Y axis from its nominal value
    void set_Y_offset (const Angle& offset);

    //! Get the offset of the feed calibrator axis from its nominal value
    Angle get_calibrator_offset () const;
    //! Set the offset of the feed calibrator axis from its nominal value
    void set_calibrator_offset (const Angle& offset);

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
