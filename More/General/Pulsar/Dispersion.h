//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Dispersion.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/31 20:34:01 $
   $Author: straten $ */

#ifndef __Pulsar_Dispersion_h
#define __Pulsar_Dispersion_h

#include "Pulsar/ColdPlasma.h"
#include "Pulsar/DispersionDelay.h"

namespace Pulsar {

  //! Corrects dispersive delays
  /*! Rotates the phase of each profile in each frequency channel to
    remove dispersive delays with respect to the reference frequency.

    \post All profiles will be phase-aligned to the reference frequency
  */
  class Dispersion : public ColdPlasma {

  public:

    //! Default constructor
    Dispersion ();

    //! Set up internal variables before execution
    void setup (Integration*);

    //! Execute the correction
    void execute (Integration*);

    //! Execute the correction for an entire Pulsar::Archive
    void execute (Archive*);

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);

    //! Set the dispersion measure
    void set_dispersion_measure (double dispersion_measure);
    //! Get the dispersion measure
    double get_dispersion_measure () const;

    //! Set the dispersion due to a change in reference wavelength
    void set_delta (const double delta);
    //! Get the dispersion due to a change in reference wavelength
    double get_delta () const;

    //! Execute the correction on the selected range
    void execute (Integration*, unsigned ichan, unsigned jchan);

  protected:

    //! The dispersion due to a change in reference wavelength
    double delta;

    //! The dispersion delay calculator
    DispersionDelay dispersion;

  };

}

#endif
