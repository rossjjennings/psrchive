//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.6 $
   $Date: 2003/04/27 10:54:38 $
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

    //! Construct from the best estimate of the average pulse profile
    ReceptionCalibrator (const Archive* archive);

    //! Destructor
    ~ReceptionCalibrator ();

    //! Add the specified pulse phase bin to the set of state constraints
    void add_state (float pulse_phase);

    //! Get the number of pulse phase bin state constraints
    unsigned get_nstate () const;

    //! Add the specified pulsar observation to the set of constraints
    void add_observation (const Archive* data);

    //! Add the specified PolnCalibrator observation to the set of constraints
    void add_PolnCalibrator (const PolnCalibrator* polncal);

    //! Add the specified FluxCalibrator observation to the set of constraints
    void add_FluxCalibrator (const FluxCalibrator* fluxcal);

    //! Return true if the model is fixed (no more data may be added)
    bool is_fixed () const;

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
    bool fixed;

    //! Solve equation for each frequency
    void fit ();

    //! Check that the model is fixed
    void check_fixed (const char* method);

    //! Add the estimate to pulsar attribute
    void add_estimate (PhaseEstimate& estimate);

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
