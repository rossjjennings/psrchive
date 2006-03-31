//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ColdPlasma.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/31 20:41:24 $
   $Author: straten $ */

#ifndef __Pulsar_ColdPlasma_h
#define __Pulsar_ColdPlasma_h

#include "Pulsar/Transformation.h"
#include "Calibration/Faraday.h"

namespace Pulsar {

  class Integration;
  class Archive;

  //! Corrections for dielectric effects in cold plasma
  /*! Corrections are performed with respect to a reference frequency.
    By default, this is the centre frequency of the Integration;
    however, it is possible to correct with respect to an arbitrary
    frequency or wavelength (including zero wavelength = infinite
    frequency).
  */
  class ColdPlasma : public Transformation<Integration> {

  public:

    //! Default constructor
    ColdPlasma ();

    //! The default Faraday rotation correction
    void transform (Integration*);

    //! Execute the correction for an entire Pulsar::Archive
    virtual void execute (Archive*);

    //! Set up internal variables before execution
    /* \post reference_frequency = Integration::get_centre_frequency. */
    virtual void setup (Integration*);

    //! Execute the correction
    /* \post All data will be corrected to the reference frequency */
    virtual void execute (Integration*) = 0;

     //! Set the reference wavelength in metres
    virtual void set_reference_wavelength (double metres);
    //! Get the reference wavelength
    double get_reference_wavelength () const;

   //! Set the reference frequency in MHz
    void set_reference_frequency (double MHz);
    //! Get the reference frequency in MHz
    double get_reference_frequency () const;

  protected:

    double reference_wavelength;

  };

}

#endif
