//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PulsarCalibrator.h

#ifndef __Pulsar_PulsarCalibrator_H
#define __Pulsar_PulsarCalibrator_H

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/Mean.h"

#include "toa.h"

#include <stdio.h>

namespace Pulsar {

  class Archive;
  class Integration;
  class PolnProfileFit;
  class ReferenceCalibrator;

  //! Implements Measurement Equation Template Matching (van Straten 2013)
  /*! This class determines the instrumental response using a pulsar
    as a polarized reference source.  The inputs are a well-calibrated
    observation of a pulsar (ideally with high signal-to-noise ratio)
    to be used as the reference source and at least one observation of
    the same source. The output is an estimate of the instrumental
    response at the epoch of the observation(s) that are fit to the
    reference source. */
  class PulsarCalibrator : public SystemCalibrator {
    
  public:
    
    //! Constructor
    PulsarCalibrator (Calibrator::Type* = 0);

    //! Destructor
    ~PulsarCalibrator ();

    //! Return true if this calibrator can be applied to the data
    bool calibrator_match (const Archive*, std::string& reason_for_not_matching);

    //! When set, pulse phase is removed from the model
    void set_fixed_phase (bool flag = true);

    //! Share a single phase estimate between all observations
    void share_phase ();

    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Get the number of pulsar harmonics (input polarization states)
    unsigned get_nstate_pulsar () const;

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Allow software to choose the maximum harmonic
    void set_choose_maximum_harmonic (bool flag = true);

    //! Get the number of harmonics in use
    unsigned get_nharmonic () const;

    //! Set the standard to which pulsar profiles will be fit
    void set_standard (const Archive* data);

    //! Solve each sub-integration (instead of global fit)
    void set_solve_each (bool = true);

    //! Unload the solution from each sub-integration
    void set_unload_each (Unloader*);

    //! Set the solution to the mean
    void update_solution ();

    //! Return the transformation to be used for precalibration
    MEAL::Complex2* get_transformation (const Archive* data,
					unsigned isub, unsigned ichan);

    //! The matrix template matching engine used to fit the specified channel
    const PolnProfileFit* get_mtm (unsigned ichan) const;

    //! Set true to detect gimbal lock when rotations are not quaternion
    bool monitor_gimbal_lock;

  protected:
    
    friend class MatrixTemplateMatching;

    //! The template/standard
    Reference::To<Pulsar::Archive> standard;

    //! Solve the measurement equation for each sub-integration
    bool solve_each;

    //! Store the solution derived from each sub-integration
    typedef std::map< unsigned, Reference::Vector<MEAL::Complex2> > Storage;
    Storage store_each;

    //! Unload the solution derived from each sub-integration
    Reference::To<Unloader> unload_each;

    //! Return a pointer to a newly constructed/initialized transformation
    MEAL::Complex2* new_transformation (unsigned ichan);

    //! The calibration model as a function of frequency
    Reference::Vector<PolnProfileFit> mtm;

    //! The reduced chi-squared as a function of frequency
    std::vector<float> reduced_chisq;

    typedef MEAL::Mean< MEAL::Complex2 > MeanXform;

    //! The array of transformation Model instances
    Reference::Vector<MeanXform> solution;

    //! Flag set when phase should be held fixed
    bool fixed_phase;

    //! Share a single phase estimate between multiple observations
    bool shared_phase;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! When set, the software will choose the maximum harmonic
    bool choose_maximum_harmonic;

    //! The maximum harmonic chosen
    unsigned chosen_maximum_harmonic;

    //! The on-pulse region
    PhaseWeight onpulse;

    //! The baseline region
    PhaseWeight baseline;

    //! Initialize the SignalPath of the specified channel
    void init_model (unsigned ichan);

    //! Solve the measurement equation for the given measurements
    void solve1 (const Calibration::CoherencyMeasurementSet& measurements);

    //! Set things up for the model in the given channel
    void setup (const Integration* data, unsigned ichan);

    //! Prepare to export the solution in current state; e.g. for plotting
    virtual void export_prepare () const;

    //! Ensure that the pulsar observation can be added to the data set
    virtual void match (const Archive*);

    //! Add data from the specified sub-integration
    virtual void add_pulsar (const Archive* data, unsigned isub);

    //! Add data from the specified channel
    virtual void add_pulsar (Calibration::CoherencyMeasurementSet&,
			     const Integration*, unsigned ichan);

    //! Add the ReferenceCalibrator observation to the set of constraints
    virtual void add_calibrator (const ReferenceCalibrator*);

  private:

    // used to communicate between solve and add_observation
    unsigned big_difference;

    //! Initialize the PolnProfileFit (mtm) model for each channel
    void build (unsigned nchan);

  };

}

#endif
