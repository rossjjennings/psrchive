//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/ReceptionCalibratorPlotter.h,v $
   $Revision: 1.6 $
   $Date: 2003/05/23 11:41:54 $
   $Author: straten $ */

#ifndef __ReceptionCalibratorPlotter_H
#define __ReceptionCalibratorPlotter_H

#include "Pulsar/PolnCalibratorPlotter.h"

namespace Pulsar {

  class ReceptionCalibrator;

  //! Pulsar::ReceptionCalibrator plotter
  class ReceptionCalibratorPlotter : public PolnCalibratorPlotter {
    
  public:

    ~ReceptionCalibratorPlotter ();

    //! Prepare to plot the Pulsar::Calibrator
    void init (const Calibrator* calibrator);

    //! Get the number of data points to plot
    unsigned get_ndat () const;

    //! Get the number of boost parameters
    unsigned get_nboost () const;

    //! Get the number of rotation parameters
    unsigned get_nrotation () const;

    //! Get the gain for the specified point
    Estimate<float> get_gain (unsigned idat);

    //! Get the gain for the specified point
    Estimate<float> get_boost (unsigned idat, unsigned iboost);

    //! Get the gain for the specified point
    Estimate<float> get_rotation (unsigned idat, unsigned irot);

    //! Plot the Stokes parameters versus parallactic angle
    void plot_constraints ();

    //! Plot the Stokes parameters versus parallactic angle
    void plot_cal_constraints ();

    //! Plot the Stokes parameters versus parallactic angle
    void plot_cal_constraints (unsigned ichan);

    //! Plot the observed Stokes parameters versus parallactic angle
    void plot_constraints (unsigned ichan, unsigned istate);

    //! Overlay the model Stokes parameters versus parallactic angle
    void plot_model (unsigned ichan, unsigned istate);

    //! Plot vertical lines over the phases used as pulsar constraints
    void plot_phase_constraints ();

  protected:

    //! Reference stored on init
    Reference::To<ReceptionCalibrator> calibrator;

  };

}

#endif
