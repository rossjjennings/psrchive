//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FluxCalibrator.h,v $
   $Revision: 1.2 $
   $Date: 2003/02/12 22:11:29 $
   $Author: straten $ */

#ifndef __FluxCalibrator_H
#define __FluxCalibrator_H

#include <string>

#include "Reference.h"
#include "Estimate.h"
#include "Types.h"

namespace Pulsar {

  //! Forward declaration
  class Archive;

  class FluxCalibrator : public Reference::Able {
    
  public:
    //! Verbosity flag
    static bool verbose;

    //! Construct from an vector of FluxCal Pulsar::Archives
    FluxCalibrator (const vector<Pulsar::Archive*>& archives);

    //! Return the flux of Hydra in mJy
    double hydra_flux_mJy (double frequency_MHz);

  protected:

    //! Filenames of Pulsar::Archives from which instance was created
    vector<string> filenames;

    //! Calibrator flux in mJy as a function of frequency
    vector< Estimate<double> > cal_flux;

    //! Temperature of system (+ sky) in mJy as a function of frequency
    vector< Estimate<double> > T_sys;

    //! Reference to the Pulsar::Archive with which this instance associates
    Reference::To<Pulsar::Archive> archive;

  };

}

#endif
