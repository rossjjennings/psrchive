//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Receiver.h,v $
   $Revision: 1.5 $
   $Date: 2004/07/03 07:38:34 $
   $Author: straten $ */

#ifndef __ReceiverExtension_h
#define __ReceiverExtension_h

#include "Pulsar/Archive.h"
#include "Jones.h"
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


    //! Different modes of feed rotational tracking
    enum Tracking {
      //! Constant feed angle
      Feed,
      //! Constant Celestial position angle
      Celestial,
      //! Constant Galactic position angle
      Galactic
    };

    //! Get the tracking mode of the receiver platform
    Tracking get_tracking_mode () const { return tracking_mode; }
    //! Set the tracking mode of the receiver platform
    void set_tracking_mode (Tracking mode) { tracking_mode = mode; }

    //! Get the position angle tracked by the receiver
    const Angle get_tracking_angle () const { return tracking_angle; }
    //! Get the position angle tracked by the receiver
    void set_tracking_angle (const Angle& angle) { tracking_angle = angle; }

    //! Get the name of the receiver
    const string get_name () const { return name; }
    //! Set the name of the receiver
    void set_name (const string& _name) { name = _name; }

    //! Get the basis of the feed receptors
    Signal::Basis get_basis () const { return basis; }
    //! Set the basis of the feed receptors
    void set_basis (Signal::Basis _basis) { basis = _basis; }

    //! Get the offset of the feed X axis with respect to the platform zero
    const Angle get_X_offset () const { return X_offset; }
    //! Set the offset of the feed X axis with respect to the platform zero
    void set_X_offset (const Angle& offset) { X_offset = offset; }

    //! Get the offset of the feed Y axis from its nominal value
    const Angle get_Y_offset () const { return Y_offset; }
    //! Set the offset of the feed Y axis from its nominal value
    void set_Y_offset (const Angle& offset) { Y_offset = offset; }

    //! Get the offset of the feed calibrator axis from its nominal value
    const Angle get_calibrator_offset () const { return calibrator_offset; }
    //! Set the offset of the feed calibrator axis from its nominal value
    void set_calibrator_offset (const Angle& o) { calibrator_offset = o; }

    //! Get the flag set when the offset of the feed has been corrected
    bool get_feed_corrected () const { return feed_corrected; }
    //! Set the flag set when the offset of the feed has been corrected
    void set_feed_corrected (bool val) { feed_corrected = val; }

    //! Get the flag set when platform to sky transformation has been corrected
    bool get_platform_corrected () const { return platform_corrected; }
    //! Set the flag set when platform to sky transformation has been corrected
    void set_platform_corrected (bool val) { platform_corrected = val; }

    //! Get the attenuator, Poln A
    float get_atten_a () const { return atten_a; }
    //! Set the attenuator, Poln A
    void set_atten_a (float atten) { atten_a = atten; }

    //! Get the attenuator, Poln B
    float get_atten_b () const { return atten_b; }
    //! Set the attenuator, Poln B
    void set_atten_b (float atten) { atten_b = atten; }

    //! Return true if the receiver configurations match
    bool match (const Receiver* receiver, string& reason) const;

    //! Return a string that describes the tracking mode
    string get_tracking_mode_string() const;

    //! Return the feed correction matrix
    Jones<double> get_correction () const;

  protected:

    //! The tracking mode of the receiver platform
    Tracking tracking_mode;

    //! The rotation angle tracked by the feed
    /*! The interpretation of this angle depends on the mode */
    Angle tracking_angle;

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
      axes and any rotation of the feed (tracking_angle) with respect to
      the platform zero have been corrected. */
    bool feed_corrected;

    //! Flag set when platform to sky transformation has been corrected
    /*! For a horizon mounted antenna, this flag should be set when
      the rotation of the platform zero, known as the vertical angle or
      parallactic angle, has been corrected. */
    bool platform_corrected;

    //! Attenuator, Poln A
    /*! The software currently does nothing with this value */
    float atten_a;

    //! Attenuator, Poln B
    /*! The software currently does nothing with this value */
    float atten_b;

  };

}

#endif
