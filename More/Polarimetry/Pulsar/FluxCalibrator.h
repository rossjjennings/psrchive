//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FluxCalibrator.h,v $
   $Revision: 1.12 $
   $Date: 2004/07/22 07:08:22 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCalibrator_H
#define __Pulsar_FluxCalibrator_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class Integration;

  class FluxCalibrator : public Calibrator {
    
    friend class FluxCalibratorInfo;

  public:
    
    //! List of possible calibration standards
    enum source {
      //! Virgo A
      Virgo,
      //! Hydra A
      Hydra,
      //! 3C353
      TCTFT,
      //! 0407-658
      OFOS,
      //! Undetermined
      Unknown
    };
    
    //! Self-calibrate flux calibrator archives before computing hi/lo ratios
    static bool self_calibrate;

    //! Default constructor
    FluxCalibrator ();

    //! Construct from a vector of FluxCal Pulsar::Archives
    FluxCalibrator (const vector<const Archive*>& archives);

    //! Return Calibrator::Flux
    Type get_type () const;

    //! Return the FluxCalibrator information
    Info* get_Info () const;
    
    //! Return the flux of 0407-658 in mJy
    double ofos_flux_mJy (double frequency_MHz);
    
    //! Return the flux of Virgo in mJy
    double virgo_flux_mJy (double frequency_MHz);
    
    //! Return the flux of Hydra in mJy
    double hydra_flux_mJy (double frequency_MHz);
    
    //! Return the flux of 3C353 in mJy
    double three_C_353_flux_mJy (double frequency_MHz);
    
    //! Return the system temperature in Kelvin
    double meanTsys ();
    
    //! Return the system temperature of a specific channel
    double Tsys (unsigned ichan);
    
    //! Add an FluxCal Pulsar::Archive to the set of constraints
    void add_observation (const Archive* archive);

    //! Calibrate the flux in the given archive
    void calibrate (Archive* archive);

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const;

  protected:

    //! Return the FluxCalibrator::source corresponding to name
    source get_RefSrc(string name);
    
    //! Calibrator flux in mJy as a function of frequency
    vector< Estimate<double> > cal_flux;

    //! Temperature of system (+ sky) in mJy as a function of frequency
    vector< Estimate<double> > T_sys;

    //! Ratio of cal hi/lo on source
    vector<Estimate<double> > ratio_on;

    //! Ratio of cal hi/lo off source
    vector<Estimate<double> > ratio_off;

    //! Create the cal_flux spectrum at the requested resolution
    void create (unsigned nchan = 0);

    //! Calculate the ratio_on and ratio_off
    void calculate ();

    //! Compute cal_flux and T_sys, given the hi/lo ratios on and off source
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
