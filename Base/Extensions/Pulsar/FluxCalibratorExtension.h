//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FluxCalibratorExtension.h,v $
   $Revision: 1.2 $
   $Date: 2004/10/08 10:05:06 $
   $Author: straten $ */

#ifndef __FluxCalibratorExtension_h
#define __FluxCalibratorExtension_h

#include "Pulsar/CalibratorExtension.h"

namespace Pulsar {

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

  protected:

    //! Calibrator flux in mJy as a function of frequency
    vector< Estimate<double> > cal_flux;

    //! Temperature of system (+ sky) in mJy as a function of frequency
    vector< Estimate<double> > T_sys;

  };
 

}

#endif
