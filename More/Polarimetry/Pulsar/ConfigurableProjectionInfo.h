//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ConfigurableProjectionInfo.h

#ifndef __Pulsar_ConfigurableProjectionInfo_H
#define __Pulsar_ConfigurableProjectionInfo_H

#include "Pulsar/Calibrator.h"
#include "Pulsar/ConfigurableProjection.h"

namespace Pulsar {

  //! Communicates ConfigurableProjection parameters to plotting routines
  class ConfigurableProjection::Info : public Calibrator::Info {

  public:

    //! Constructor
    Info (const ConfigurableProjection* projection);
    
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
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;

    //! Group N parameters in one class
    void set_together (unsigned N) { together = N; }

    //! Return the colour index
    int get_colour_index (unsigned iclass, unsigned iparam) const;

    //! Return the graph marker
    int get_graph_marker (unsigned iclass, unsigned iparam) const;

  protected:

    //! The ConfigurableProjection to be plotted
    Reference::To<const ConfigurableProjection> projection;

    //! Indeces of parameters with estimates
    std::vector<unsigned> parameters;

    //! Names of parameters with estimates
    std::vector<std::string> names;

    //! Number of parameters plotted in one panel
    unsigned together;

  };

}

#endif

