//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.9 $
   $Date: 2003/04/29 10:27:25 $
   $Author: straten $ */

#ifndef __ReceptionCalibrator_H
#define __ReceptionCalibrator_H

#include "Calibrator.h"
#include "Calibration/SAtPEquation.h"
#include "Calibration/Parallactic.h"
#include "Calibration/StokesState.h"

namespace Pulsar {

  class Archive;
  class Integration;
  class PolnCalibrator;
  class FluxCalibrator;

  class PhaseEstimate;

  //! Uses the SAtPEquation to represent and fit for the system response
  /*! The ReceptionCalibrator implements a technique of single dish
    polarimetric self-calibration.  This class requires a number of
    constraints, which are provided in through the add_observation,
    add_PolnCalibrator, and add_FluxCalibrator methods. */
  class ReceptionCalibrator : public Calibrator {
    
  public:

    friend class ReceptionCalibratorPlotter;

    //! Construct with optional first pulsar archive
    ReceptionCalibrator (const Archive* archive = 0);

    //! Set the number of polynomial coefficients in SingleAxis(t)
    void set_ncoef (unsigned ncoef);
      
    //! Add the specified pulse phase bin to the set of state constraints
    void add_state (unsigned pulse_phase_bin);

    //! Get the number of pulse phase bin state constraints
    unsigned get_nstate () const;

    //! Add the specified pulsar observation to the set of constraints
    void add_observation (const Archive* data);

    //! Add the specified PolnCalibrator observation to the set of constraints
    void add_PolnCalibrator (const PolnCalibrator* polncal);

    //! Add the specified FluxCalibrator observation to the set of constraints
    void add_FluxCalibrator (const FluxCalibrator* fluxcal);

    //! Solve equation for each frequency
    void solve ();

    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);

  protected:

    //! SingleAxis(t)Polar Equation as a function of frequency
    vector<Calibration::SAtPEquation> equation;

    //! Calibrator state as a function of frequency
    vector<Calibration::StokesState> calibrator;

    //! Uncalibrated estimate of pulsar polarization as a function of phase
    vector<PhaseEstimate> pulsar;

    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! Uncalibrated best estimate of the average pulse profile
    Reference::To<const Archive> uncalibrated;

    //! Flag set after the fit method has been called
    bool is_fit;

    //! The number of polynomial coefficients in SingleAxis(t)
    unsigned ncoef;

    //! Flag set after set_ncoef is called
    bool ncoef_set;

    //! Epoch of the first observation
    MJD start_epoch;

    //! Epoch of the last observation
    MJD end_epoch;

    //! Flag set after successful call to add_PolnCalibrator
    bool includes_PolnCalibrator;

    //! Flag set after successful call to add_FluxCalibrator
    bool includes_FluxCalibrator;

    //! Check that the model is ready 
    void check_ready (const char* method);

    //! Initialization performed using the first observation added
    void initial_observation (const Archive* data);

    //! Add the estimate to pulsar attribute
    void init_estimate (PhaseEstimate& estimate);

    //! Add Integration data to the MeasuredState vector
    /*! Data is taken from the specified frequency channel and phase bin.
      \retval bins the measured states to which an entry will be appended
      \param estimate contains the bin number and a running mean estimate
      \param ichan the frequency channel
      \param data the Integration data */
    void add_data (vector<Calibration::MeasuredState>& bins,
		   PhaseEstimate& estimate,
		   unsigned ichan,
		   const Integration* data);

  };

  class PhaseEstimate {

  public:

    //! Construct with the specified bin from Archive
    PhaseEstimate (unsigned ibin = 0) { phase_bin = ibin; }

    //! Update each state with the mean
    void update_state();

    //! Best estimate of Stokes parameters as a function of frequency
    vector< MeanEstimate<Stokes<double>, double> > mean;

    //! Model fit of added to equation as a function of frequency
    vector< Calibration::StokesState > state;

    //! Phase bin from which
    unsigned phase_bin;

  };

}

#endif
