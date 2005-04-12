//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver_Field.h,v $
   $Revision: 1.2 $
   $Date: 2005/04/12 06:44:28 $
   $Author: straten $ */

#ifndef __Receiver_Field_h
#define __Receiver_Field_h

#include "Pulsar/Receiver_Native.h"

namespace Pulsar {

  //! Receiver state in which the field orientation may be specified
  /*! The orientation of the electric field vector that induces equal and
    in-phase responses in orthogonal receptors depends upon the basis.  In
    the linear basis, it has an orientation of 45 degrees.  In the circular
    basis, 0 degrees.  Therefore, if the field_orientation attribute is set,
    the interpretation of the orientation will become basis dependent. */
  class Receiver_Field : public Pulsar::Receiver_Native {

  public:

    //! Default constructor
    Receiver_Field ();

    //! Get the orientation of the basis about the line of sight
    Angle get_orientation () const;
    //! Set the orientation of the basis about the line of sight
    void set_orientation (const Angle& angle);

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
