//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/RobustStepFinder.h

#ifndef __Pulsar_RobustStepFinder_H
#define __Pulsar_RobustStepFinder_H

#include "Pulsar/SystemCalibratorStepFinder.h"

namespace Pulsar
{
  class RobustStepFinder : public SystemCalibrator::StepFinder
  {
    //! multiple of median standard deviation considered to be a step
    float step_threshold;

    //! Number of mutually consistent points required on either side of step
    unsigned depth;

    //! The calibrator on which operations are performed
    SystemCalibrator* calibrator;

    //! count of consistent sub-integrations before each pulsar sub-integration
    std::vector<unsigned> psr_before;
    
    //! count of consistent sub-integrations after each pulsar sub-integration
    std::vector<unsigned> psr_after;

    //! count of consistent sub-integrations before each CAL sub-integration
    std::vector<unsigned> cal_before;
    
    //! count of consistent sub-integrations after each CAL sub-integration
    std::vector<unsigned> cal_after;

    //! Stokes parameters used during consistency comparisons
    std::vector<unsigned> compare;
    
    template<typename Container>
    void count_consistent (const Container& container,
			   std::vector<unsigned>& before,
			   std::vector<unsigned>& after,
			   bool wedge);

    template<typename Container>
    void remove_inconsistent (Container& container,
			      std::vector<unsigned>& before,
			      std::vector<unsigned>& after);

    //! Search for steps using counts of consistent sub-integrations
    void find_steps (std::vector<unsigned>& steps,
		     const std::vector<unsigned>& before,
		     const std::vector<unsigned>& after);

    template<class Container>
    void add_steps (std::vector<MJD>& steps,
		    std::vector<unsigned>& step_sub,
		    Container& data,
		    bool align_to_cal = 0);

    // adjust epochs and ids to align to calibrator observations
    bool align_to_cal (MJD& i_epoch, std::string& i_id,
		       MJD& j_epoch, std::string& j_id);
      
    //! Search for steps using counts of consistent pulsar sub-integrations
    /*! Steps are aligned to epochs of calibrator sub-integrations */

    void find_steps_pulsar (std::vector<MJD>& steps);
    void find_steps_calibrator (std::vector<MJD>& steps);

    //! Insert steps using the specified transfrom
    void insert_steps (std::vector<MJD>& steps,
		       Calibration::VariableBackend* xform);

    void remove_extra_calibrators ();
    void remove_outliers ();

    //! Search for and insert steps in instrumental response
    void insert_steps ();

  public:

    //! Default constructor
    RobustStepFinder (float threshold = 3.0)
    {
      step_threshold = threshold;
      depth = 3;
    }

    void set_step_threshold (float val) { step_threshold = val; }
    
    void process (SystemCalibrator*);

  };
}

#endif
