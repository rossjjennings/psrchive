//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CalibratorPlotter.h,v $
   $Revision: 1.4 $
   $Date: 2003/09/11 21:15:41 $
   $Author: straten $ */

#ifndef __CalibratorPlotter_H
#define __CalibratorPlotter_H

#include "Reference.h"

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

    //! Number of panels (vertical)
    unsigned npanel;

  };

}

#endif
