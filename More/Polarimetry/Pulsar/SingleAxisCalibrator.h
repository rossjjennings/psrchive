//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SingleAxisCalibrator.h,v $
   $Revision: 1.6 $
   $Date: 2003/09/04 10:21:33 $
   $Author: straten $ */

#ifndef __SingleAxisCalibrator_H
#define __SingleAxisCalibrator_H

#include "PolnCalibrator.h"
#include "Calibration/SingleAxis.h"

namespace Pulsar {

  //! Uses SingleAxis to represent the system response
  class SingleAxisCalibrator : public PolnCalibrator {
    
  public:

    friend class SingleAxisCalibratorPlotter;
    friend class ReceptionCalibrator;

    //! Construct from an vector of PolnCal Pulsar::Archives
    SingleAxisCalibrator (const vector<Archive*>& a) : PolnCalibrator (a) {}

    //! Construct from an single PolnCal Pulsar::Archive
    SingleAxisCalibrator (const Archive* a) : PolnCalibrator (a) {}

    //! Destructor
    ~SingleAxisCalibrator ();

  protected:

    //! Return the system response as determined by the SingleAxis
    virtual Jones<double> solve (const vector<Estimate<double> >& hi,
				 const vector<Estimate<double> >& lo,
				 unsigned ichan);

    //! Resize the space used to store SingleAxis parameters
    virtual void resize_parameters (unsigned nchan);

    //! SingleAxis representation of the instrument
    vector<Calibration::SingleAxis> model;

  };

}

#endif
