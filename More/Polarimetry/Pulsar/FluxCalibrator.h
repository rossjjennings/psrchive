//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FluxCalibrator.h,v $
   $Revision: 1.6 $
   $Date: 2003/05/23 10:28:28 $
   $Author: pulsar $ */

#ifndef __FluxCalibrator_H
#define __FluxCalibrator_H

#include <string>

#include "Calibrator.h"
#include "Estimate.h"
#include "Types.h"

namespace Pulsar {

  class Integration;

  class FluxCalibrator : public Calibrator {
    
    friend class FluxCalibratorPlotter;

  public:
    //! Self-calibrate flux calibrator archives before computing hi/lo ratios
    static bool self_calibrate;

    //! Default constructor
    FluxCalibrator ();

    //! Construct from an vector of FluxCal Pulsar::Archives
    FluxCalibrator (const vector<Archive*>& archives);

    //! Return the flux of Hydra in mJy
    double hydra_flux_mJy (double frequency_MHz);

    //! Add an FluxCal Pulsar::Archive to the set of constraints
    void add_observation (const Archive* archive);

    //! Calibrate the flux in the given archive
    void calibrate (Archive* archive);

  protected:

    //! Filenames of Pulsar::Archives from which instance was created
    vector<string> filenames;

    //! Calibrator flux in mJy as a function of frequency
    vector< Estimate<double> > cal_flux;

    //! Temperature of system (+ sky) in mJy as a function of frequency
    vector< Estimate<double> > T_sys;

    //! Reference to the Pulsar::Archive with which this instance associates
    Reference::To<const Archive> calibrator;

    //! Ratio of cal hi/lo on hydra
    vector<Estimate<double> > ratio_on;

    //! Ratio of cal hi/lo off hydra
    vector<Estimate<double> > ratio_off;

    //! Create the cal_flux spectrum at the requested resolution
    void create (unsigned nchan);

    //! Calculate the ratio_on and ratio_off
    void calculate ();

    //! Compute cal_flux and T_sys, given the hi/lo ratios on and off hydra
    void calculate (vector<Estimate<double> >& on,
		    vector<Estimate<double> >& off);

    //! Calibrate a single sub-integration
    void calibrate (Integration* subint);

  private:

    vector<MeanEstimate<double> > mean_ratio_on;
    vector<MeanEstimate<double> > mean_ratio_off;

    //! Set true after call to calculate
    bool calculated;

  };

}

#endif
