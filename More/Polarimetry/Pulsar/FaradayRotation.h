//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FaradayRotation.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/31 20:42:28 $
   $Author: straten $ */

#ifndef __Pulsar_FaradayRotation_h
#define __Pulsar_FaradayRotation_h

#include "Pulsar/ColdPlasma.h"
#include "Calibration/Faraday.h"

namespace Pulsar {

  //! Corrects Faraday rotation
  /*!  Rotates the polarization profiles in each frequency channel
    about the line of sight to remove Faraday rotation with respect to
    the reference frequency.

    \pre The noise contribution to Stokes Q and U should have been removed.
    \pre The Integration must have full polarimetric information.

    \post All profiles will have a position angle aligned to the
          reference frequency
  */
  class FaradayRotation : public ColdPlasma {

  public:

    //! Default constructor
    FaradayRotation ();

    //! Set up internal variables before execution
    void setup (Integration*);

    //! Execute the correction
    void execute (Integration*);

    //! Execute the correction for an entire Pulsar::Archive
    void execute (Archive*);

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);

    //! Set the rotation measure
    void set_rotation_measure (double rotation_measure);
    //! Get the rotation measure
    double get_rotation_measure () const;

    //! Set the rotation due to a change in reference wavelength
    void set_delta (const Jones<double>& delta);
    //! Get the rotation due to a change in reference wavelength
    Jones<double> get_delta () const;

    //! Execute the correction on the selected range
    void execute (Integration*, unsigned ichan, unsigned jchan);

  protected:

    //! The Faraday rotation transformation
    Calibration::Faraday faraday;

    //! The rotation due to a change in reference wavelength
    Jones<double> delta;

  };

}

#endif
