//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolarCalibrator.h,v $
   $Revision: 1.2 $
   $Date: 2003/04/04 08:44:44 $
   $Author: straten $ */

#ifndef __PolarCalibrator_H
#define __PolarCalibrator_H

#include "PolnCalibrator.h"
#include "PolarModel.h"

namespace Pulsar {

  //! Uses PolarModel to represent the system response
  class PolarCalibrator : public PolnCalibrator {
    
  public:

    friend class PolarCalibratorPlotter;

    //! Construct from an vector of PolnCal Pulsar::Archives
    PolarCalibrator (const vector<Archive*>& a) : PolnCalibrator (a) {}

    //! Construct from an single PolnCal Pulsar::Archive
    PolarCalibrator (const Archive* archive) : PolnCalibrator (archive) {}

    //! Destructor
    ~PolarCalibrator ();

  protected:

    //! Return the system response as determined by the PolarModel
    virtual Jones<double> solve (const vector<Estimate<double> >& hi,
				 const vector<Estimate<double> >& lo,
				 unsigned ichan);

    //! Resize the space used to store PolarModel parameters
    virtual void resize_parameters (unsigned nchan);

    //! PolarModel representation of the instrument
    vector<Calibration::PolarModel> model;

  };

}

#endif
