//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver.h,v $
   $Revision: 1.4 $
   $Date: 2004/06/21 15:50:09 $
   $Author: straten $ */

#ifndef __ReceiverExtension_h
#define __ReceiverExtension_h

#include "Pulsar/Archive.h"
#include "Angle.h"

namespace Pulsar {
  
  //! Contains information about the receiver and receiver platform

  class Receiver : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    Receiver ();

    //! Copy constructor
    Receiver (const Receiver& extension);

    //! Operator =
    const Receiver& operator= (const Receiver& extension);

    //! Destructor
    ~Receiver ();

    //! Clone method
    Receiver* clone () const { return new Receiver(*this); }

    //! Return true if the receiver configurations match
    bool match (const Receiver* receiver, string& reason) const;

    //! Return true if the receiver frame is correct
    bool get_correct () const;

    //! Return a string that describes the tracking mode
    string get_tracking_mode_string() const;

    //! Correct the receiver frame of the given archive
    static void correct (Archive* archive);

    //////////////////////////////////////////////////////////////////////
    //
    // Information about the receiver platform
    //

    //! Different modes of feed rotational tracking
    enum Tracking {
      //! Constant feed angle
      Feed,
      //! Constant Celestial position angle
      Celestial,
      //! Constant Galactic position angle
      Galactic
    };

    //! The tracking mode of the receiver platform
    Tracking mode;

    //! The rotation angle tracked by the feed
    /*! The interpretation of this angle depends on the mode */
    Angle tracking_angle;

    //////////////////////////////////////////////////////////////////////
    //
    // Information about the receiver
    //

    //! Name of the receiver
    string name;

    //! Basis of the feed receptors
    Signal::Basis basis;

    //! Offset of the feed X axis with respect to the platform zero
    Angle X_offset;
    
    //! Offset of the feed Y axis from its nominal value
    /*! Nominally, the Y axis is offset from the X axis by 90 degrees.
      However it is also common to encounter systems in which the Y
      axis is offset by -90 degrees, owing to unaccounted reflections,
      phase delays, and inconsistent definitions.  Nominally, this
      parameter should be set to zero. */
    Angle Y_offset;

    //! Offset of the calibrator axis from its nominal value
    /*! Nominally, the position angle of the calibrator reference
      signal (linear noise diode) is 45 degrees (measured from X
      toward Y).  Nominally, this parameter should be set to zero. */
    Angle calibrator_offset;

    //! Flag set when the offset of the feed has been corrected
    /*! This flag should be set when the offset of the feed X and Y
      axes and any rotation of the feed (feed_angle) with respect to
      the platform zero have been corrected. */
    bool feed_offset_corrected;

    //! Flag set when vertical offset has been corrected
    /*! This flag should be set when the offset of the platform zero,
      known as the vertical angle or parallactic angle, has been
      corrected. */
    bool vertical_offset_corrected;

    //! Attenuator, Poln A
    /*! The software currently does nothing with this value */
    float atten_a;

    //! Attenuator, Poln B
    /*! The software currently does nothing with this value */
    float atten_b;

  };

}

#endif
