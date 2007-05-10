//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/ReceptionCalibratorPlotter.h,v $
   $Revision: 1.19 $
   $Date: 2007/05/10 04:23:24 $
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

    //! Return the calibrator to be plotted
    const ReceptionCalibrator* get_calibrator () const { return calibrator; }

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
