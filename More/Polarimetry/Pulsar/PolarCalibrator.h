//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolarCalibrator.h,v $
   $Revision: 1.6 $
   $Date: 2003/09/11 21:15:41 $
   $Author: straten $ */

#ifndef __PolarCalibrator_H
#define __PolarCalibrator_H

#include "Pulsar/ArtificialCalibrator.h"
#include "Calibration/Polar.h"

namespace Pulsar {

  //! Uses PolarModel to represent the system response
  class PolarCalibrator : public ArtificialCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    PolarCalibrator (const Archive* archive);

    //! Destructor
    ~PolarCalibrator ();

    //! Return the PolarAxisCalibrator information
    Info* get_Info () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ArtificialCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new Calibration::Polar instance
    ::Calibration::Transformation* solve (const vector<Estimate<double> >& hi,
					  const vector<Estimate<double> >& lo);


  };

}

#endif
