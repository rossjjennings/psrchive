//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/UnloadJones.h

#ifndef __UnloadJones_H
#define __UnloadJones_H

#include "Pulsar/SystemCalibratorUnloader.h"

namespace Pulsar
{
  //! Unloads SystemCalibrator as a 2-D array of Jones matrices
  /*! Jones matrix coordinates are printed as a function of time and frequency.
      The output can be read with "pac -Q" */
  class UnloadJones : public SystemCalibrator::Unloader
  {
  public:

    //! Default constructor
    UnloadJones ();

    //! Unload the SystemCalibrator solution using the current settings
    void unload (SystemCalibrator*);

  };

}

#endif
