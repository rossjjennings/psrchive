//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_CalibratorInfo_h
#define __Pulsar_CalibratorInfo_h

#include "Pulsar/MultiFrequency.h"
#include "Pulsar/CalibratorParameter.h"

namespace Pulsar {

  //! Plots every class of model parameters from Calibrator::Info
  /*! 
    CalibratorInfo creates and manages a CalibratorParameter instance
    for each class of model parameters and divides the plot viewport
    into sub-panels as needed.
  */
  class CalibratorInfo : public MultiFrequency
  {
  public:

    //! Default constructor
    CalibratorInfo ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the CalibratorInfo class
    class Interface : public TextInterface::To<CalibratorInfo> {
    public:
      Interface (CalibratorInfo* = 0);
    };

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    void set_between_panels (float x) { between_panels = x; }
    float get_between_panels () const { return between_panels; }

    void set_panels (const std::string& x) { panels = x; }
    std::string get_panels () const { return panels; }

    void set_calibrator_stokes (bool x) { calibrator_stokes = x; }
    bool get_calibrator_stokes () const { return calibrator_stokes; }

    void set_calibrator_stokes_degree (bool x);
    bool get_calibrator_stokes_degree () const;

    void set_reduced_chisq (bool x) { reduced_chisq = x; }
    bool get_reduced_chisq () const { return reduced_chisq; }

    void set_intrinsic_crosspol_ratio (bool x) { intrinsic_crosspol_ratio = x; }
    bool get_intrinsic_crosspol_ratio () const { return intrinsic_crosspol_ratio; }

    void set_constant_gain (bool x) { constant_gain = x; }
    bool get_constant_gain () const { return constant_gain; }

    void set_outlier_threshold (float t) { outlier_threshold = t; }
    float get_outlier_threshold () const { return outlier_threshold; }

    //! Set the sub-integration Idex
    void set_subint (const Index& isub) { subint = isub; }
    //! Get the sub-integration Idex
    Index get_subint () const { return subint; }

  protected:

    //! Separate panels for each class of calibrator parameters
    std::vector< Reference::To<CalibratorParameter> > parameter;

    //! spacing between plot panels
    float between_panels;

    //! panels to be plotted
    std::string panels;

    //! plot the calibrator Stokes parameters
    bool calibrator_stokes;

    //! plot the calibrator Stokes parameters w/ degree of polarization
    bool calibrator_stokes_degree;
    
    //! plot the goodness-of-fit statistic
    bool reduced_chisq;

    //! plot the intrinsic cross-polarization ratio
    bool intrinsic_crosspol_ratio;

    //! plot constant gain flux calibrator information
    bool constant_gain;

    //! threshold used to detect outliers when creating SingleAxis calibrator
    float outlier_threshold;

    // Sub-integration from which to derive a solution
    Index subint;
    

  };

}

#endif
