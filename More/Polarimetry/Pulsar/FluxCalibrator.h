//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FluxCalibrator.h,v $
   $Revision: 1.19 $
   $Date: 2004/12/27 14:21:56 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCalibrator_H
#define __Pulsar_FluxCalibrator_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class Integration;
  class FluxCalibratorDatabase;

  class FluxCalibrator : public Calibrator {
    
    friend class FluxCalibratorInfo;

  public:
    
    //! Self-calibrate flux calibrator archives before computing hi/lo ratios
    static bool self_calibrate;

    //! Default constructor
    FluxCalibrator (const Archive* archive = 0);

    //! Return Calibrator::Flux
    Type get_type () const;

    //! Return the FluxCalibrator information
    Info* get_Info () const;
    
    //! Return a new FluxCalibratorExtension
    CalibratorExtension* new_Extension () const;

    //! Return the system temperature in Kelvin
    double meanTsys ();
    
    //! Return the system temperature of a specific channel
    double Tsys (unsigned ichan);
    
    //! Add a FluxCal Pulsar::Archive to the set of constraints
    void add_observation (const Archive* archive);

    //! Set the database containing flux calibrator information
    void set_database (const FluxCalibratorDatabase* database);

    //! Calibrate the flux in the given archive
    void calibrate (Archive* archive);

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const;

  protected:

    friend class FluxCalibratorExtension;

    //! Flux calibrator database
    Reference::To<const FluxCalibratorDatabase> database;

    //! Calibrator flux in mJy as a function of frequency
    std::vector< Estimate<double> > cal_flux;

    //! Temperature of system (+ sky) in mJy as a function of frequency
    std::vector< Estimate<double> > T_sys;

    //! Ratio of cal hi/lo on source
    std::vector<Estimate<double> > ratio_on;

    //! Ratio of cal hi/lo off source
    std::vector<Estimate<double> > ratio_off;

    //! Create the cal_flux spectrum at the requested resolution
    void create (unsigned nchan = 0);

    //! Resize the T_sys and cal_flux vector
    void resize (unsigned required_nchan);

    //! Calculate the ratio_on and ratio_off
    void calculate ();

    //! Compute cal_flux and T_sys, given the hi/lo ratios on and off source
    void calculate (std::vector<Estimate<double> >& on,
		    std::vector<Estimate<double> >& off);

    //! Calibrate a single sub-integration
    void calibrate (Integration* subint);

  private:

    std::vector<MeanEstimate<double> > mean_ratio_on;
    std::vector<MeanEstimate<double> > mean_ratio_off;

    //! Set true after call to calculate
    bool calculated;

    //! FluxCal-On data available
    bool have_on;

    //! FluxCal-Off data available
    bool have_off;

    //! Initialize attributes
    void init ();

  };

}

#endif
