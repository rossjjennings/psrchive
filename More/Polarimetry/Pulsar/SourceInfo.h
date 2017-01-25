//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SourceInfo.h

#ifndef __Pulsar_SourceInfo_H
#define __Pulsar_SourceInfo_H

#include "Pulsar/Calibrator.h"
#include "Pulsar/SourceEstimate.h"

namespace Pulsar {

  //! Communicates Calibrator Stokes parameters
  class SourceInfo : public Calibrator::Info
  {

  public:

    //! Constructor
    SourceInfo (const std::vector<Calibration::SourceEstimate>& source);
    
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

    //! Plot all Stokes parameters in one panel
    void set_together (bool);

    //! Set the label printed on the y-axis
    void set_label (const std::string&);

    //! Set the title
    void set_title (const std::string&);

  protected:
    
    //! The SourceEstimate to be plotted
    const std::vector<Calibration::SourceEstimate>& source;

    //! Plot all Stokes parameters in one panel
    bool together;

    //! The label printed on the y-axis
    std::string label;

    //! The title
    std::string title;

  };

}

#endif
