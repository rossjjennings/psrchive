//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FaradayRotation.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/31 22:57:35 $
   $Author: straten $ */

#ifndef __Pulsar_FaradayRotation_h
#define __Pulsar_FaradayRotation_h

#include "Pulsar/ColdPlasma.h"

#include "Calibration/Faraday.h"
#include "Pulsar/DeFaraday.h"

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
  class FaradayRotation : public ColdPlasma<Calibration::Faraday,DeFaraday> {

  public:

    //! Default constructor
    FaradayRotation ();

    //! Return the rotation measure
    double correction_measure (Integration*);

    //! Return the identity matrix
    Jones<double> get_identity () { return 1; }

    //! Faraday rotate each profile by the correction
    void apply (Integration*, unsigned channel);

    //! Set the Faraday rotation correction attributes in the Archive
    void execute (Archive*);

    //! Set the rotation measure
    void set_rotation_measure (double rotation_measure)
    { set_measure (rotation_measure); }
      
    //! Get the rotation measure
    double get_rotation_measure () const
    { return get_measure (); }

  };

}

#endif
