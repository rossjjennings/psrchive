//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FluxCalibratorExtension.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/24 20:43:29 $
   $Author: straten $ */

#ifndef __FluxCalibratorExtension_h
#define __FluxCalibratorExtension_h

#include "Pulsar/CalibratorExtension.h"

namespace Pulsar {

  class FluxCalibrator;
  //! Flux Calibrator Extension
  /*! This Extension implements the storage of FluxCalibrator data. */
  
  class FluxCalibratorExtension : public CalibratorExtension {
    
    friend class FluxCalibrator;

  public:
    
    //! Default constructor
    FluxCalibratorExtension ();

    //! Copy constructor
    FluxCalibratorExtension (const FluxCalibratorExtension&);

    //! Operator =
    const FluxCalibratorExtension& operator= (const FluxCalibratorExtension&);

    //! Destructor
    ~FluxCalibratorExtension ();

    //! Clone method
    FluxCalibratorExtension* clone () const
    { return new FluxCalibratorExtension( *this ); }

    //! Construct from a FluxCalibrator instance
    FluxCalibratorExtension (const FluxCalibrator*);

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);

    //! Set the number of receptors
    void set_nreceptor (unsigned nreceptor);

    //! Get the number of receptors
    unsigned get_nreceptor () const;

    //! Set the system equivalent flux density of the specified channel
    void set_S_sys (unsigned chan, unsigned receptor, const Estimate<double>&);
    //! Get the system equivalent flux density of the specified channel
    Estimate<double> get_S_sys (unsigned chan, unsigned receptor) const;

    //! Set the calibrator flux density of the specified channel
    void set_S_cal (unsigned chan, unsigned receptor, const Estimate<double>&);
    //! Get the calibrator flux density of the specified channel
    Estimate<double> get_S_cal (unsigned chan, unsigned receptor) const;

  protected:

    //! System equivalent flux density in each receptor and frequency channel
    /*! in mJy */
    std::vector< std::vector< Estimate<double> > > S_sys;

    //! Calibrator flux density in each receptor and frequency channel
    /*! in mJy */
    std::vector< std::vector< Estimate<double> > > S_cal;


  };
 

}

#endif
