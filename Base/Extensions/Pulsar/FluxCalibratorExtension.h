//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/FluxCalibratorExtension.h,v $
   $Revision: 1.4 $
   $Date: 2005/06/12 00:01:11 $
   $Author: redwards $ */

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

    //! Set the system temperature of the specified channel
    void set_T_sys (unsigned ichan, const Estimate<double>& T_sys);
    //! Get the system temperature of the specified channel
    Estimate<double> get_T_sys (unsigned ichan) const;

    //! Set the calibrator flux of the specified channel
    void set_cal_flux (unsigned ichan, const Estimate<double>& cal_flux);
    //! Get the calibrator flux of the specified channel
    Estimate<double> get_cal_flux (unsigned ichan) const;

  protected:

    //! Calibrator flux in mJy as a function of frequency
    vector< Estimate<double> > cal_flux;

    //! Temperature of system (+ sky) in mJy as a function of frequency
    vector< Estimate<double> > T_sys;

  };
 

}

#endif
