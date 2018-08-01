//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluxCalibratorData.h

#ifndef __Pulsar_FluxCalibratorData_H
#define __Pulsar_FluxCalibratorData_H

#include "Pulsar/FluxCalibrator.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarConstant.h"
#include "Types.h"

namespace Pulsar {

  //! Flux calibration data for each receptor
  class FluxCalibrator::Data : public Reference::Able
  {

  public:

    //! Default constructor
    Data ();

    //! Construct from known flux densities for each receptor
    Data ( const std::vector< Estimate<double> >& S_sys,
	   const std::vector< Estimate<double> >& S_cal );
    
    //! Return the flux densities for each receptor
    void get ( std::vector< Estimate<double> >& S_sys,
	       std::vector< Estimate<double> >& S_cal ) const;
    
    //! Set the number of receptors
    void set_nreceptor (unsigned);
    
    //! Get the number of receptors
    unsigned get_nreceptor () const;

    //! Integrate an observation of the reference source
    virtual void integrate (Signal::Source source, unsigned ireceptor,
			    const Estimate<double>& cal_hi,
			    const Estimate<double>& cal_lo) = 0;
    
    //! Set the flux density of the standard candle
    void set_S_std (double S_std);
    
    //! Get the flux density of the standard candle
    double get_S_std () const;
    
    //! Set the data validity flag
    void set_valid (bool f = true) { valid = f; }
    
    //! Get the data validity flag
    bool get_valid () const { return valid && calculated; }
    
    //! Return the total system equivalent flux density
    Estimate<double> get_S_sys () const;
    
    //! Return the total calibrator flux density
    Estimate<double> get_S_cal () const;
    
    //! Return the system equivalent flux density in the specified receptor
    Estimate<double> get_S_sys (unsigned receptor) const;
    
    //! Return the calibrator flux density in the specified receptor
    Estimate<double> get_S_cal (unsigned receptor) const;
    
  protected:
    
    //! Flag set when data are valid
    bool valid;
    
    //! Flux density of the standard candle
    double S_std;
    
    //! Calibrator flux density in each receptor
    mutable std::vector< Estimate<double> > S_cal;
    
    //! System equivalent flux density in each receptor
    mutable std::vector< Estimate<double> > S_sys;

    //! Flag set when S_cal and S_sys have been computed
    mutable bool calculated;
    
    //! Calulate S_cal and S_sys for both receptors
    void calculate () const;
    
    //! Compute the fluxes of the reference source and system
    virtual void compute (unsigned ireceptor,
			  Estimate<double>& S_cal,
			  Estimate<double>& S_sys) = 0;
    
  };

  //! Flux calibration policy when gain varies between FluxCal-On and Off
  class FluxCalibrator::VariableGain : public FluxCalibrator::Data
  {

  public:

    //! Default constructor
    VariableGain ();
    
    void integrate (Signal::Source source, unsigned ireceptor,
		    const Estimate<double>& cal_hi,
		    const Estimate<double>& cal_lo);
    
    //! Construct from known flux densities
    VariableGain ( const std::vector< Estimate<double> >& S_sys,
		   const std::vector< Estimate<double> >& S_cal );
    
  protected:
    
    //! Ratio of cal hi/lo on source in each receptor
    std::vector< MeanEstimate<double> > mean_ratio_on;
    
    //! Ratio of cal hi/lo off source in each receptor
    std::vector< MeanEstimate<double> > mean_ratio_off;

    //! Add to the mean hi/lo ratio on source for the specified receptor
    void add_ratio_on (unsigned receptor, Estimate<double>& ratio_on);
    
    //! Add to the mean hi/lo ratio off source for the specified receptor
    void add_ratio_off (unsigned receptor, Estimate<double>& ratio_on);
    
    //! Compute the fluxes of the reference source and system
    void compute (unsigned ireceptor,
		  Estimate<double>& S_cal,
		  Estimate<double>& S_sys);

  private:
    
    /* 
       Use the ScalarMath class to calculate the variances.
       These are static because they cost a bit during construction.
    */
    MEAL::ScalarParameter ratio_on;
    MEAL::ScalarParameter ratio_off;
    MEAL::ScalarConstant unity;
    
    MEAL::ScalarMath flux_cal;
    MEAL::ScalarMath flux_sys;

    void init ();
  };
  
}

#endif
