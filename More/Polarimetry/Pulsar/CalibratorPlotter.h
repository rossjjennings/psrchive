//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CalibratorPlotter.h,v $
   $Revision: 1.2 $
   $Date: 2003/05/02 14:30:35 $
   $Author: straten $ */

#ifndef __CalibratorPlotter_H
#define __CalibratorPlotter_H

#include "Reference.h"
#include "Estimate.h"

namespace Pulsar {

  class Calibrator;

  //! Pure virtual base class of Pulsar::Calibrator plotters
  class CalibratorPlotter : public Reference::Able {
    
  public:
    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    CalibratorPlotter ();
    
    //! Destructor
    virtual ~CalibratorPlotter ();

    //! Plot the Pulsar::Calibrator
    virtual void plot (const Calibrator* calibrator);

    //! Prepare to plot the Pulsar::Calibrator
    virtual void init (const Calibrator* calibrator) = 0;

    //! Get the number of data points to plot
    virtual unsigned get_ndat () const = 0;

    //! Get the number of boost parameters
    virtual unsigned get_nboost () const = 0;

    //! Get the number of rotation parameters
    virtual unsigned get_nrotation () const = 0;

    //! Get the gain for the specified point
    virtual Estimate<float> get_gain (unsigned idat) = 0;

    //! Get the gain for the specified point
    virtual Estimate<float> get_boost (unsigned idat, unsigned iboost) = 0;

    //! Get the gain for the specified point
    virtual Estimate<float> get_rotation (unsigned idat, unsigned irot) = 0;

  };

}

#endif
