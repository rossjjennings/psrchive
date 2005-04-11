//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver_Field.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/11 10:26:15 $
   $Author: straten $ */

#ifndef __Receiver_Field_h
#define __Receiver_Field_h

#include "Pulsar/Receiver_Native.h"

namespace Pulsar {

  //! Directly implements Receiver State
  class Receiver_Field : public Pulsar::Receiver_Native {

  public:

    //! Default constructor
    Receiver_Field ();

    //! Get the orientation of the basis about the line of sight
    Angle get_orientation () const;
    //! Set the orientation of the basis about the line of sight
    void set_orientation (const Angle& angle);

    //! Copy the state from another
    void copy (const State*);
    
    //! Get the orientation of the equal in-phase electric field vector
    Angle get_field_orientation () const;
    //! Set the orientation of the equal in-phase electric field vector
    void set_field_orientation (const Angle& celestial_position_angle);

  protected:
    
    //! Set true if the basis forms a right-handed coordinate system
    bool field_orientation;

  };

}

#endif
