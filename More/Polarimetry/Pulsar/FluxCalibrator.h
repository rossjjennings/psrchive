//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FluxCalibrator.h,v $
   $Revision: 1.7 $
   $Date: 2003/09/11 21:15:41 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCalibrator_H
#define __Pulsar_FluxCalibrator_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class Integration;

  class FluxCalibrator : public Calibrator {
    
    friend class FluxCalibratorInfo;

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

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const;

    //! Return the FluxCalibrator information
    Info* get_Info () const;

  protected:

    //! Calibrator flux in mJy as a function of frequency
    vector< Estimate<double> > cal_flux;

    //! Temperature of system (+ sky) in mJy as a function of frequency
    vector< Estimate<double> > T_sys;

    //! Ratio of cal hi/lo on hydra
    vector<Estimate<double> > ratio_on;

    //! Ratio of cal hi/lo off hydra
    vector<Estimate<double> > ratio_off;

    //! Create the cal_flux spectrum at the requested resolution
    void create (unsigned nchan = 0);

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
