//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnCalibrator.h,v $
   $Revision: 1.2 $
   $Date: 2003/02/13 14:08:33 $
   $Author: straten $ */

#ifndef __PolnCalibrator_H
#define __PolnCalibrator_H

#include <string>

#include "Calibrator.h"
#include "Estimate.h"
#include "Types.h"

namespace Pulsar {

  //! Forward declaration
  class Archive;

  class PolnCalibrator : public Calibrator {
    
  public:
    //! Verbosity flag
    static bool verbose;

    //! Construct from an vector of PolnCal Pulsar::Archives
    PolnCalibrator (const vector<Archive*>& archives);

    //! Construct from an single PolnCal Pulsar::Archive
    PolnCalibrator (const Archive* archives);

    //! Calibrate the polarization of the given archive
    void calibrate (Archive* archive);

  protected:

    //! Filenames of Pulsar::Archives from which instance was created
    vector<string> filenames;

    //! Differential gain, \f$ \beta \f$, as a function of frequency
    vector< Estimate<double> > gain;

    //! Differential phase, \f$ \Phi_I \f$, as a function of frequency
    vector< Estimate<double> > phase;

    //! Reference to the Pulsar::Archive from which this instance was created
    Reference::To<const Archive> archive;

    void calculate (vector<vector<Estimate<double> > >& cal_hi,
		    vector<vector<Estimate<double> > >& cal_lo);

  };

}

#endif
