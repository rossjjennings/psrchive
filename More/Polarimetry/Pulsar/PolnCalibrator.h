//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnCalibrator.h,v $
   $Revision: 1.1 $
   $Date: 2003/02/12 19:26:47 $
   $Author: straten $ */

#ifndef __PolnCalibrator_H
#define __PolnCalibrator_H

#include <string>

#include "Reference.h"
#include "Estimate.h"
#include "Types.h"

namespace Pulsar {

  //! Forward declaration
  class Archive;

  class PolnCalibrator : public Reference::Able {
    
  public:
    //! Verbosity flag
    static bool verbose;

    //! Construct from an vector of PolnCal Pulsar::Archives
    PolnCalibrator (const vector<Pulsar::Archive*>& archives);

  protected:

    //! Filenames of Pulsar::Archives from which instance was created
    vector<string> filenames;

    //! Differential gain as a function of frequency
    vector< Estimate<double> > gain;

    //! Differential phase as a function of frequency
    vector< Estimate<double> > phase;

    //! Reference to the Pulsar::Archive from which this instance was created
    Reference::To<Pulsar::Archive> archive;

  };

}

#endif
