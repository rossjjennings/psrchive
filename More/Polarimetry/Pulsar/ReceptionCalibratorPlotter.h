//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/ReceptionCalibratorPlotter.h,v $
   $Revision: 1.13 $
   $Date: 2003/10/28 08:48:50 $
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
    void plot_constraints (unsigned ichan, unsigned istate);

    //! Plot the specified plot, overlaying the ichan/ipol model
    void plot_stokes (EstimatePlotter& plotter, unsigned iplot,
		      unsigned ichan, unsigned ipol, float spacing = 0.0);

    //! Overlay the model Stokes parameters versus parallactic angle
    void plot_model (unsigned ichan, unsigned ipol, unsigned npt = 100);

    //! Plot vertical lines over the phases used as pulsar constraints
    void plot_phase_constraints ();

  protected:

    //! Reference stored on init
    Reference::To<ReceptionCalibrator> calibrator;

  };

}

#endif
