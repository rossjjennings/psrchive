//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnCalibrator.h,v $
   $Revision: 1.3 $
   $Date: 2003/02/13 16:39:11 $
   $Author: straten $ */

#ifndef __PolnCalibrator_H
#define __PolnCalibrator_H

#include <string>

#include "Calibrator.h"
#include "Estimate.h"
#include "Jones.h"
#include "Types.h"

namespace Pulsar {

  class Integration;

  //! Implements differenctial gain and phase corrections
  /*! This class implements the calibration of instrumental differential
    gain and phase, as previously implemented by tcedar and cal_arch.
    One major improvement: this calibration step no longer requires a flux
    calibrator in order to work.  The calibrated archive will have its flux
    normalized by the calibrator flux, such that the FluxCalibrator class
    need only multiply the archive by the calibrator flux in mJy */
  class PolnCalibrator : public Calibrator {
    
  public:
    //! If true, then the human-readable parameters are kept
    static bool store_parameters;

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
    vector< Estimate<double> > boost_q;

    //! Differential phase, \f$ \Phi_I \f$, as a function of frequency
    vector< Estimate<double> > rotation_q;

    //! Instrumental gain, \f$ G \f$, in instrumental flux units
    vector< Estimate<double> > gain;

    //! Intensity of off-pulse (system + sky), in CAL flux units
    vector< Estimate<double> > baseline;

    //! Reference to the Pulsar::Archive from which this instance was created
    Reference::To<const Archive> calibrator;

    //! Jones matrix frequency response of the instrument
    vector< Jones<float> > jones;

    //! Create the Jones matrix frequency response at the requested resolution
    void create (unsigned nchan);

    //! Calculate the Jones matrix frequency response from the CAL hi and lo
    void calculate (vector<vector<Estimate<double> > >& cal_hi,
		    vector<vector<Estimate<double> > >& cal_lo);

    //! Calibrate a single sub-integration
    void calibrate (Integration* subint);
  };

}

#endif
