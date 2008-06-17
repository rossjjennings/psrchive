//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PulsarCalibrator.h,v $
   $Revision: 1.29 $
   $Date: 2008/06/17 01:35:59 $
   $Author: straten $ */

#ifndef __Pulsar_PulsarCalibrator_H
#define __Pulsar_PulsarCalibrator_H

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/Mean.h"

#include <stdio.h>

namespace Pulsar {

  class Archive;
  class Integration;
  class PolnProfileFit;
  class ReferenceCalibrator;

  //! Uses PolnProfileFit to determine the system response
  /*! The PulsarCalibrator implements a technique of polarimetric
    calibration using a well-determined source.  This class requires a
    polarimetric standard and another observation of the same source.  */
  class PulsarCalibrator : public SystemCalibrator {
    
  public:
    
    //! Constructor
    PulsarCalibrator (Calibrator::Type model = Calibrator::Britton);

    //! Destructor
    ~PulsarCalibrator ();

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Allow software to choose the maximum harmonic
    void set_choose_maximum_harmonic (bool flag = true);

    //! Get the number of harmonics in use
    unsigned get_nharmonic () const;

    //! Normalize each Stokes vector by the mean on-pulse invariant
    void set_normalize_by_invariant (bool flag = true);

    //! Set the standard to which pulsar profiles will be fit
    void set_standard (const Archive* data);

    //! Set the flag to solve for each observation (instead of global fit)
    void set_solve_each (bool flag = true);

    //! Set the solution to the mean
    void update_solution ();

    //! File to which arrival time estimates should be written
    void set_tim_file (FILE* fptr) { tim_file = fptr; }

    //! The matrix template matching engine used to fit the specified channel
    const PolnProfileFit* get_mtm (unsigned ichan) const;

    //! Set true to detect gimbal lock when rotations are not quaternion
    bool monitor_gimbal_lock;

  protected:
    
    //! Return a pointer to a newly constructed/initialized transformation
    MEAL::Complex2* new_transformation (unsigned ichan);

    //! The calibration model as a function of frequency
    std::vector< Reference::To<PolnProfileFit> > mtm;

    //! The reduced chi-squared as a function of frequency
    std::vector<float> reduced_chisq;

    //! The phase shift estimate as a function of frequency
    std::vector< Estimate<double> > phase_shift;

    typedef MEAL::Mean< MEAL::Complex2 > MeanXform;

    //! The array of transformation Model instances
    std::vector< Reference::To<MeanXform> > solution;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! When set, the software will choose the maximum harmonic
    bool choose_maximum_harmonic;

    //! The maximum harmonic chosen
    unsigned chosen_maximum_harmonic;

    //! Normalize the Stokes parameters by the invariant interval
    bool normalize_by_invariant;

    //! Solve the measurement equation on each call to add_observation
    bool solve_each;

    //! The on-pulse region
    PhaseWeight on_pulse;

    //! The baseline region
    PhaseWeight baseline;

    //! Initialize the StandardModel of the specified channel
    void init_model (unsigned ichan);

    //! Solve the measurement equation for the given channel
    void solve1 (const Integration* data, unsigned ichan);

    //! Set things up for the model in the given channel
    unsigned setup (const Integration* data, unsigned ichan);

    //! Prepare to export the solution in current state; e.g. for plotting
    virtual void export_prepare () const;

    //! Ensure that the pulsar observation can be added to the data set
    virtual void match (const Archive*);

    //! Add data from the specified sub-integration
    virtual void add_pulsar (const Archive* data, unsigned isub);

    //! Add data from the specified channel
    virtual void add_pulsar (Calibration::CoherencyMeasurementSet&,
			     const Integration*, unsigned ichan);

  private:

    // used to communicate between solve and add_observation
    unsigned big_difference;

    //! File to which arrival time estimates should be written
    FILE* tim_file;

    //! Archive instance that is currently in use
    const Archive* archive;

    //! Build the arrays
    void build (unsigned nchan);

  };

}

#endif
