//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SimPolnTiming.h,v $
   $Revision: 1.1 $
   $Date: 2005/07/12 23:21:06 $
   $Author: straten $ */

#ifndef __Pulsar_SimPolnTiming_h
#define __Pulsar_SimPolnTiming_h

#include "Pulsar/SyntheticPolnProfile.h"
#include "Pulsar/PolnProfileFit.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/Gaussian.h"

namespace Pulsar {

  class Archive;

  //! Simulates full-Stokes arrival time estimation
  class SimPolnTiming {

  public:

    //! Default constructor
    SimPolnTiming ();

    //! Destructor
    ~SimPolnTiming ();

    //! The synthetic polarimetric profile generator
    SyntheticPolnProfile generate;

    //! The arrival time estimation algorithm
    PolnProfileFit fit;

    //! The gaussian total intensity profile
    MEAL::Gaussian gaussian;

    //! The rotating vector model used to describe the position angle
    MEAL::RotatingVectorModel rvm;

    //! Number of bins in pulse profile
    unsigned nbin;

    //! Set the phase of the standard profile centre
    void set_centre (double);

    //! Relative noise in standard profile
    float standard_noise;

    //! Difference between magnetic axis and line of sight
    float rvm_beta;

    //! Set the archive to be used as a standard
    void set_standard (Archive*);

    //! Number of simulated observations for each step in simulation
    unsigned iterations;

    //! Relative noise in simulated observations
    float measurement_noise;

    //! Phase offset of simulated observations
    float offset;

    //! Vary the line of sight
    bool vary_line_of_sight;

    //! Number of steps when varying the RVM orientation
    unsigned slope_steps;
    double slope_min, slope_max;
    double slope;

    //! Number of steps when varying the Gaussian width
    unsigned width_steps;
    double width_min, width_max;

    //! Number of steps when varying the differental gain
    unsigned dgain_steps;
    double dgain_min, dgain_max;

    //! Differential gain to be applied to observations
    double diff_gain;

    //! Run the simulation, outputting results to cout
    void run_simulation ();

    //! Title written over plot
    std::string title;

    //! Output values using Estimate operator <<
    bool as_Estimate;

  protected:

    class Result {
    public:
      Estimate<double> I_mean;
      Estimate<double> S_mean;
      Estimate<double> I_err;
      Estimate<double> S_err;
    };

    //! One loop in the simulation
    void one_loop (unsigned level);

    //! One step in the simulation
    Result one_step ();

    //! The data to use as a standard
    Reference::To<Pulsar::Archive> archive;

  };

}

#endif
