//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/FluxCalibrator.h,v $
   $Revision: 1.29 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCalibrator_H
#define __Pulsar_FluxCalibrator_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class Integration;
  class StandardCandles;

  //! Calibrates flux using standard candles and artificial sources
  class FluxCalibrator : public Calibrator {
    
  public:

    //! FluxCalibrator parameter communication
    class Info : public Calibrator::Info {
      
    public:
      //! Constructor
      Info (const FluxCalibrator* cal) { instance = cal; }
 
      //! Return the number of parameter classes
      unsigned get_nclass () const { return 2; }
    
      //! Return the name of the specified class
      const char* get_name (unsigned iclass) const;
    
      //! Return the number of parameters in the specified class
      unsigned get_nparam (unsigned iclass) const;
      
      //! Return the estimate of the specified parameter
      Estimate<float> get_param (unsigned ichan, unsigned iclass,
				 unsigned iparam) const;

    protected:
    
      Reference::To<const FluxCalibrator> instance;
      
    };

    //! Default constructor
    FluxCalibrator (const Archive* archive = 0);

    //! Destructor
    ~FluxCalibrator ();

    //! Return Calibrator::Flux
    Type get_type () const;

    //! Return the FluxCalibrator information
    FluxCalibrator::Info* get_Info () const;
    
    //! Return a new FluxCalibratorExtension
    CalibratorExtension* new_Extension () const;

    //! Return the system temperature in Kelvin
    double meanTsys ();
    
    //! Return the system temperature of a specific channel
    double Tsys (unsigned ichan);
    
    //! Add a FluxCal Pulsar::Archive to the set of constraints
    void add_observation (const Archive* archive);

    //! Set the database containing flux calibrator information
    void set_database (const StandardCandles* database);

    //! Calibrate the flux in the given archive
    void calibrate (Archive* archive);

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const;

    //! Get the number of receptors in the calibrator
    unsigned get_nreceptor () const;

    //! Return true if the flux scale for the specified channel is valid
    bool get_valid (unsigned ch) const;

    //! Set the flux scale invalid flag for the specified channel
    void set_invalid (unsigned ch);

  protected:

    friend class FluxCalibratorExtension;
    friend class Info;

    //! Flux calibrator database
    Reference::To<const StandardCandles> database;

    //! Create the cal_flux spectrum at the requested resolution
    void create (unsigned nchan = 0);

    //! Calculate the ratio_on and ratio_off
    void calculate ();

    //! Calibrate a single sub-integration
    void calibrate (Integration* subint);

    class Data;

    //! Flux calibrator data for each frequency channel
    std::vector< Data > data;

    //! Resize the data vector
    void resize (unsigned nchan, unsigned nreceptor);

    //! The absolute gain used to calibrate the archive data
    std::vector< float > gain;

    //! Resize the gain vector
    void resize (unsigned required_nchan);

  private:

    //! Set true after call to calculate
    bool calculated;

    //! FluxCal-On data available
    bool have_on;

    //! FluxCal-Off data available
    bool have_off;

    //! Initialize attributes
    void init ();

    //! Set up the data array with standard candle flux density
    void setup ();

  };

}

#endif
