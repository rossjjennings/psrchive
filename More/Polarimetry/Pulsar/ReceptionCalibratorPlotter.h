//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/ReceptionCalibratorPlotter.h,v $
   $Revision: 1.1 $
   $Date: 2003/05/02 14:30:35 $
   $Author: straten $ */

#ifndef __ReceptionCalibratorPlotter_H
#define __ReceptionCalibratorPlotter_H

#include "CalibratorPlotter.h"

namespace Pulsar {

  class ReceptionCalibrator;

  //! Pulsar::ReceptionCalibrator plotter
  class ReceptionCalibratorPlotter : public CalibratorPlotter {
    
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

  protected:

    //! Reference stored on init
    Reference::To<const ReceptionCalibrator> calibrator;

  };

}

#endif
