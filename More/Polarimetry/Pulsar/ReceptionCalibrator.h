//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.18 $
   $Date: 2003/05/22 06:47:12 $
   $Author: pulsar $ */

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

  class SourceEstimate;
  class PolarEstimate;

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

    //! Get the number of pulsar phase bin input polarization states
    unsigned get_nstate_pulsar () const;

    //! Get the total number of input polarization states (pulsar and cal)
    unsigned get_nstate () const;

    //! Get the number of frequency channels
    unsigned get_nchan () const;

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
    vector<Calibration::SAtPEquation*> equation;

    //! Calibrator state as a function of frequency
    vector<Calibration::StokesState> calibrator;

    //! The index by which calibrator observations are identified
    unsigned calibrator_state_index;

    //! Best estimate of polar decomposition as a function of frequency
    vector<PolarEstimate> receiver;

    //! Uncalibrated estimate of pulsar polarization as a function of phase
    vector<SourceEstimate> pulsar;

    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! Uncalibrated best estimate of the average pulse profile
    Reference::To<const Archive> uncalibrated;

    //! The number of polynomial coefficients in SingleAxis(t)
    unsigned ncoef;

    //! Flag set after set_ncoef is called
    bool ncoef_set;

    //! Epoch of the first observation
    MJD start_epoch;

    //! Epoch of the last observation
    MJD end_epoch;

    //! Minimum and maximum values of parallactic angle (informational)
    float PA_min, PA_max;

    //! Signal path assigned to PolnCalibrator
    unsigned PolnCalibrator_path;

    //! Signal path assigned to add_FluxCalibrator
    unsigned FluxCalibrator_path;

    //! Set the initial guesses and update the reference epoch
    void initialize ();

    //! Check that the model is ready to receive additional constraints
    void check_ready (const char* method, bool init = true);

    //! Initialization performed using the first observation added
    void initial_observation (const Archive* data);

    //! Add the estimate to pulsar attribute
    void init_estimate (SourceEstimate& estimate);

    //! Add Integration data to the MeasuredState vector
    /*! Data is taken from the specified frequency channel and phase bin.
      \retval bins the measured states to which an entry will be appended
      \param estimate contains the bin number and a running mean estimate
      \param ichan the frequency channel
      \param data the Integration data */
    void add_data (vector<Calibration::MeasuredState>& bins,
		   SourceEstimate& estimate,
		   unsigned ichan,
		   const Integration* data);

  private:
    //! Flag set after the solve method has been called
    bool is_fit;

     //! Flag set after the initialize method has been called
    bool is_initialized;

  };

  class SourceEstimate {

  public:

    //! Construct with the specified bin from Archive
    SourceEstimate (unsigned ibin = 0) { phase_bin = ibin; }

    //! Update each state with the mean
    void update_state();

    //! Best estimate of Stokes parameters as a function of frequency
    vector< MeanEstimate<Stokes<double>, double> > mean;

    //! Model of Stokes parameters added to equation as a function of frequency
    vector< Calibration::StokesState > state;

    //! Phase bin from which
    unsigned phase_bin;

  };

  class PolarEstimate {

  public:

    //! Best estimate of receiver gain
    MeanEstimate<double> gain;

    //! Best estimate of receiver boost
    MeanEstimate<double> boostGibbs[3];

    //! Best estimate of receiver rotations
    MeanEstimate<double> rotationEuler[3];

    //! Add the Polar Model to the current best estimate
    void integrate (const Calibration::Polar& model);

    //! Update the Polar Model with the currect best estimate
    void update (Calibration::Polar* model);

  };

}

#endif
