//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SingleAxisCalibrator.h,v $
   $Revision: 1.7 $
   $Date: 2003/09/11 21:15:42 $
   $Author: straten $ */

#ifndef __Pulsar_SingleAxisCalibrator_H
#define __Pulsar_SingleAxisCalibrator_H

#include "Pulsar/ArtificialCalibrator.h"
#include "Calibration/SingleAxis.h"

namespace Pulsar {

  //! Uses SingleAxis to represent the system response
  class SingleAxisCalibrator : public ArtificialCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    SingleAxisCalibrator (const Archive* archive);

    //! Destructor
    ~SingleAxisCalibrator ();

    //! Return the SingleAxisCalibrator information
    Info* get_Info () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ArtificialCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new Calibration::SingleAxis instance
    ::Calibration::Transformation* solve (const vector<Estimate<double> >& hi,
					  const vector<Estimate<double> >& lo);

  };

}

#endif
