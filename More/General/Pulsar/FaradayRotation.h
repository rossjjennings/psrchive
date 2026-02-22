//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FaradayRotation.h

#ifndef __Pulsar_FaradayRotation_h
#define __Pulsar_FaradayRotation_h

#include "Pulsar/ColdPlasma.h"

#include "Pulsar/Faraday.h"
#include "Pulsar/BirefringenceHistory.h"

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
  class FaradayRotation : public ColdPlasma<Calibration::Faraday,BirefringenceHistory>
  {

  public:

    //! Default constructor
    FaradayRotation ();

    //! Return the rotation measure
    /*! As returned by psredit -c rm */
    double get_relative_measure (const Integration*) const override;

    //! Return the auxiliary rotation measure
    /*! As returned by psredit -c int:aux:rm */
    double get_absolute_measure (const Integration*) const override;

    //! Return true if the rotation measure has been corrected with respect to centre frequency
    /*! As returned by psredit -c rmc */
    bool get_relative_corrected (const Integration*) const override;

    //! Return true if the auxiliary rotation measure has been corrected with respect to centre frequency
    /*! As returned by psredit -c aux:rmc */
    bool get_absolute_corrected (const Integration*) const override;

    //! Return the identity matrix
    Jones<double> get_identity () const override { return 1; }

    //! Combine matrices
    void combine (Jones<double>& res, const Jones<double>& J) const override { res *= J; }

    //! Invert the Faraday rotation in the specified polarization
    void apply (Integration*, unsigned channel, Jones<double> rotation) override;

    //! Apply the current correction to all sub-integrations in an archive
    void execute (Archive*) override;

    //! Undo the correction
    void revert (Archive*) override;

    //! Set the rotation measure
    void set_rotation_measure (double rotation_measure)
    { relative.set_measure (rotation_measure); }
      
    //! Get the rotation measure
    double get_rotation_measure () const
    { return relative.get_measure (); }

  };

}

#endif
