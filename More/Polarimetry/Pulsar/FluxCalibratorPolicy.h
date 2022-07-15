//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluxCalibratorPolicy.h

#ifndef __Pulsar_FluxCalibratorPolicy_H
#define __Pulsar_FluxCalibratorPolicy_H

#include "Pulsar/FluxCalibrator.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/ScalarParameter.h"
#include "MEAL/ScalarConstant.h"
#include "Types.h"

namespace Pulsar {

  //! Flux calibration data for each receptor
  class FluxCalibrator::Policy : public Reference::Able
  {

  public:

    //! Default constructor
    Policy ();

    //! Return a new default constructed copy of self
    virtual Policy* clone () const = 0;

    //! Integrate an observation of the reference source
    virtual void integrate (Signal::Source source, unsigned ireceptor,
			    const Estimate<double>& cal_hi,
			    const Estimate<double>& cal_lo) = 0;
        
    //! Get the flux densities for each receptor
    void get ( std::vector< Estimate<double> >& S_sys,
	       std::vector< Estimate<double> >& S_cal ) const;

    //! Set the flux densities for each receptor
    void set ( const std::vector< Estimate<double> >& S_sys,
	       const std::vector< Estimate<double> >& S_cal );

    //! Set the number of receptors
    virtual void set_nreceptor (unsigned);
    
    //! Get the number of receptors
    unsigned get_nreceptor () const;

    //! Set the flux density of the standard candle
    void set_S_std (double S_std);
    
    //! Get the flux density of the standard candle
    double get_S_std () const;
    
    //! Set the data validity flag
    void set_valid (bool f = true) { valid = f; }
    
    //! Get the data validity flag
    bool get_valid () const { return valid; }

    //! Return true when a solution is available
    bool get_solution_available () const;

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
    mutable bool valid;
    
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

  //! Used when gain varies between FluxCal-On and Off observations
  class FluxCalibrator::VariableGain : public FluxCalibrator::Policy
  {

  public:

    //! Default constructor
    VariableGain ();

    VariableGain* clone () const;
    
    void integrate (Signal::Source source, unsigned ireceptor,
		    const Estimate<double>& cal_hi,
		    const Estimate<double>& cal_lo);
    
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
  };

  //! Used when gain remains constant between FluxCal-On and Off observations
  class FluxCalibrator::ConstantGain : public FluxCalibrator::Policy
  {

  public:

    //! Default constructor
    ConstantGain ();

    ConstantGain* clone () const;

    //! Set the number of receptors
    void set_nreceptor (unsigned);
    
    void integrate (Signal::Source source, unsigned ireceptor,
		    const Estimate<double>& cal_hi,
		    const Estimate<double>& cal_lo);

    //! Get the scale for the specified receptor
    Estimate<double> get_scale (unsigned ireceptor) const;
    //! Get the scale for each receptor
    void get_scale ( std::vector< Estimate<double> >& );

    //! Set the scale for the specified receptor
    void set_scale (unsigned ireceptor, const Estimate<double>&);
    //! Set the scale for each receptor
    void set_scale ( const std::vector< Estimate<double> >& );

    //! Get the gain for the specified receptor
    Estimate<double> get_gain (unsigned ireceptor) const;

    //! Get the gain ratio for the specified receptor
    Estimate<double> get_gain_ratio (unsigned ireceptor) const;
    //! Get the gain ratio for each receptor
    void get_gain_ratio ( std::vector< Estimate<double> >& );

    //! Set the gain ratio for the specified receptor
    void set_gain_ratio (unsigned ireceptor, const Estimate<double>&);
    //! Set the gain ratio for each receptor
    void set_gain_ratio ( const std::vector< Estimate<double> >& );

    Estimate<double> get_scale () const;
    Estimate<double> get_gain () const;

  protected:
    
    //! Mean of on-source observations with noise diode emitting
    std::vector< MeanEstimate<double> > mean_hi_on;
    //! Mean of on-source observations with noise diode off
    std::vector< MeanEstimate<double> > mean_lo_on;
    
    //! Mean of off-source observations with noise diode emitting
    std::vector< MeanEstimate<double> > mean_hi_off;
    //! Mean of off-source observations with noise diode off
    std::vector< MeanEstimate<double> > mean_lo_off;
    
    //! Compute the fluxes of the reference source and system
    void compute (unsigned ireceptor,
		  Estimate<double>& S_cal,
		  Estimate<double>& S_sys);

    void invalidate (unsigned ireceptor);

    std::vector< Estimate<double> > scale;
    std::vector< Estimate<double> > gain_ratio;
    
  private:
    
    /* 
       Use the ScalarMath class to calculate the variances.
    */
    MEAL::ScalarParameter ratio_hi;
    MEAL::ScalarParameter ratio_lo;
    MEAL::ScalarConstant unity;
    
    MEAL::ScalarMath flux_cal;
    MEAL::ScalarMath flux_sys;

  };
}

#endif
