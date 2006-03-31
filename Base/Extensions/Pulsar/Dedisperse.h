//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Dedisperse.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/31 19:00:54 $
   $Author: straten $ */

#ifndef __Pulsar_Dedisperse_h
#define __Pulsar_Dedisperse_h

#include "Pulsar/Integration.h"

namespace Pulsar {
  
  //! Stores parameters used to correct dispersion in each Integration
  class Dedisperse : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    Dedisperse ();

    //! Copy constructor
    Dedisperse (const Dedisperse& extension);

    //! Assignment operator
    const Dedisperse& operator= (const Dedisperse& extension);
    
    //! Destructor
    ~Dedisperse ();

    //! Clone method
    Dedisperse* clone () const { return new Dedisperse( *this ); }

    //! Set the dispersion measure
    void set_dispersion_measure (double dispersion_measure);
    //! Get the dispersion measure
    double get_dispersion_measure () const;

    //! Set the reference wavelength in metres
    void set_reference_wavelength (double metres);
    //! Get the reference wavelength
    double get_reference_wavelength () const;

    //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency
    double get_reference_frequency () const;

  protected:

    //! The dispersion measure
    double dispersion_measure;

    //! The reference wavelength in metres
    double reference_wavelength;

  };
  
}

#endif

