//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FaradayRotation.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/17 13:34:50 $
   $Author: straten $ */

#ifndef __Pulsar_FaradayRotation_h
#define __Pulsar_FaradayRotation_h

#include "Pulsar/Transformation.h"
#include "Calibration/Faraday.h"

namespace Pulsar {

  class Integration;
  class Archive;

  //! Corrects Faraday rotation in an Integration
  /*!  Rotates the polarization profiles in each frequency channel
    about the line of sight to remove Faraday rotation with respect to
    a reference frequency.  By default, this is the centre frequency
    of the Integration; however, it is possible to correct with
    respect to an arbitrary frequency or wavelength (including zero
    wavelength / infinite frequency).

    \pre The noise contribution to Stokes Q and U should have been removed.
    \pre The Integration must have full polarimetric information.

    \post All profiles will have a position angle aligned to the
          reference frequency (by default, equal to that returned by
          Integration::get_centre_frequency).
  */
  class FaradayRotation : public Transformation<Integration> {

  public:

    //! Default constructor
    FaradayRotation ();

    //! The default Faraday rotation correction
    void transform (Integration*);

    //! Set up internal variables before execution
    void setup (Integration*);

    //! Set the rotation measure
    void set_rotation_measure (double rotation_measure);
    //! Get the rotation measure
    double get_rotation_measure () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);
    //! Get the reference wavelength
    double get_reference_wavelength () const;

    //! Execute the correction
    void execute (Integration*);

    //! Execute the correction for an entire Pulsar::Archive
    void execute (Archive*);

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
