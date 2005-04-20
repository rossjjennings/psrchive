//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/ReceptionCalibratorPlotter.h,v $
   $Revision: 1.16 $
   $Date: 2005/04/20 07:42:06 $
   $Author: straten $ */

#ifndef __ReceptionCalibratorPlotter_H
#define __ReceptionCalibratorPlotter_H

#include "Pulsar/CalibratorPlotter.h"
#include "Estimate.h"

class EstimatePlotter;

namespace Pulsar {

  class ReceptionCalibrator;

  //! Pulsar::ReceptionCalibrator plotter
  class ReceptionCalibratorPlotter : public CalibratorPlotter {
    
  public:

    ReceptionCalibratorPlotter (ReceptionCalibrator* calibrator);

    ~ReceptionCalibratorPlotter ();

    //! Plot the calibrator as a function of frequency
    void plotcal ();

    //! Plot the Stokes parameters versus parallactic angle
    void plot_cal_constraints (unsigned ichan);

    //! Plot the observed Stokes parameters versus parallactic angle
    void plot_psr_constraints (unsigned ichan, unsigned istate);

    //! Plot vertical lines over the phases used as pulsar constraints
    void plot_phase_constraints ();

    //! Cause the above to plot the residual
    void set_plot_residual (bool val) { plot_residual = val; }

  protected:

    //! Plot the observed Stokes parameters versus parallactic angle
    void plot_constraints (unsigned ichan, unsigned istate, unsigned ipath);

    //! Reference stored on init
    Reference::To<ReceptionCalibrator> calibrator;

    //! Plotting methods will plot the residual
    bool plot_residual;

  };

}

#endif
