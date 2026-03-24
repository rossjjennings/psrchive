//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PolnCalibratorInfo.h

#ifndef __Pulsar_PolnCalibratorInfo_H
#define __Pulsar_PolnCalibratorInfo_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar {

  class PolnCalibrator::Info : public Calibrator::Info
  {

  public:

    //! Factory returns a suitable instance
    static PolnCalibrator::Info* create (const PolnCalibrator* calibrator);
    
    //! Constructor
    Info (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the number of frequency channels
    unsigned get_nchan () const;

    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_label (unsigned iclass) const;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass, unsigned iparam) const;
    
    //! Return the colour index
    int get_colour_index (unsigned iclass, unsigned iparam) const;
    
    //! Return the graph marker
    int get_graph_marker (unsigned iclass, unsigned iparam) const;
    
  protected:
    
    //! The PolnCalibrator to be plotted
    Reference::To<const PolnCalibrator> calibrator;

    //! The number of parameters in the PolnCalibrator transformation
    unsigned nparam;
    
  };

}

#endif

