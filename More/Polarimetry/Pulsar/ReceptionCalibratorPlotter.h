//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/ReceptionCalibratorPlotter.h,v $
   $Revision: 1.14 $
   $Date: 2003/12/26 08:45:17 $
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
    void plot_constraints (unsigned ichan, unsigned istate, unsigned ipath);

    //! Plot vertical lines over the phases used as pulsar constraints
    void plot_phase_constraints ();

  protected:

    //! Reference stored on init
    Reference::To<ReceptionCalibrator> calibrator;

  };

}

#endif
