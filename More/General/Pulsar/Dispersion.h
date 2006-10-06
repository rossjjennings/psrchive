//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Dispersion.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_Dispersion_h
#define __Pulsar_Dispersion_h

#include "Pulsar/ColdPlasma.h"
#include "Pulsar/DispersionDelay.h"
#include "Pulsar/Dedisperse.h"

namespace Pulsar {

  //! Corrects dispersive delays
  /*! Rotates the phase of each profile in each frequency channel to
    remove dispersive delays with respect to the reference frequency.

    \post All profiles will be phase-aligned to the reference frequency
  */
  class Dispersion : public ColdPlasma<DispersionDelay,Dedisperse> {

  public:

    //! Default constructor
    Dispersion ();

    //! Return the dispersion measure
    double correction_measure (Integration*);

    //! Return zero delay
    double get_identity () { return 0; }

    //! Phase rotate each profile by the correction
    void apply (Integration*, unsigned channel);

    //! Set the dedispersion attributes in the Archive
    void execute (Archive*);

    //! Set the dispersion measure
    void set_dispersion_measure (double dispersion_measure)
    { set_measure (dispersion_measure); }
      
    //! Get the dispersion measure
    double get_dispersion_measure () const
    { return get_measure (); }

  };

}

#endif
