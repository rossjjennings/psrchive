//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ConvertMJD.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 20:45:59 $
   $Author: straten $ */

#ifndef __Calibration_ConvertMJD_Header
#define __Calibration_ConvertMJD_Header

#include "MEAL/Axis.h"
#include "MJD.h"

namespace Calibration {

  //! Converts Argument type from MJD to double
  class ConvertMJD : public MEAL::Axis<double> {

  public:

    //! Default constructor
    ConvertMJD ();

    //! Set the epoch
    void set_epoch (const MJD& epoch);

    //! Set the reference MJD
    void set_reference_epoch (const MJD& reference_epoch);

    //! Set the scale to be used during conversion
    void set_scale (double scale);

  protected:

    //! The reference epoch
    MJD reference_epoch;

    //! The scale used during conversion
    double scale;

  };

}

#endif

