//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluxCalManagerInfo.h

#ifndef __Pulsar_FluxCalManagerInfo_H
#define __Pulsar_FluxCalManagerInfo_H

#include "Pulsar/ReceptionCalibrator.h"

namespace Pulsar {

  //! Communicates FluxCalManager parameters to plotting routines
  class FluxCalManagerInfo : public Calibrator::Info {

  public:

    //! Constructor
    FluxCalManagerInfo (const ReceptionCalibrator* calibrator);
    
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

    //! Return the colour index of the specified parameter
    int get_colour_index (unsigned iclass, unsigned iparam) const;

  protected:

    //! The ReceptionCalibrator with FluxCalManager data to be plotted
    Reference::To<const ReceptionCalibrator> calibrator;

    //! The number of different classes of data
    unsigned nclass;

    //! The number of FluxCalOn classes
    unsigned nclass_on;
  };

}

#endif
