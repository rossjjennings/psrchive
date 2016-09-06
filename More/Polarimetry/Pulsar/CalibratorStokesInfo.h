//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/CalibratorStokesInfo.h

#ifndef __Pulsar_CalibratorStokesInfo_H
#define __Pulsar_CalibratorStokesInfo_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class CalibratorStokes;

  //! Communicates CalibratorStokes parameters to plotting routines
  class CalibratorStokesInfo : public Calibrator::Info {

  public:

    //! Constructor
    CalibratorStokesInfo (const CalibratorStokes* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the number of frequency channels
    unsigned get_nchan () const;

    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;

    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;

    //! Group all three calibrator Stokes parameters in one class
    void set_together (bool flag) { together = flag; }

    //! Add an additional class for the calibrator degree of polarization
    void set_degree (bool flag) { degree = flag; }
    
  protected:

    //! The CalibratorStokes to be plotted
    Reference::To<const CalibratorStokes> calibrator_stokes;

    //! Plot all of the Stokes parameters in one panel
    bool together;

    //! Plot the degree of polarization in a fourth panel
    bool degree;

  };

}

#endif
