//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SourceInfo.h,v $
   $Revision: 1.2 $
   $Date: 2008/04/07 00:38:18 $
   $Author: straten $ */

#ifndef __Pulsar_SourceInfo_H
#define __Pulsar_SourceInfo_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  class SourceEstimate;

  //! Communicates Calibrator Stokes parameters
  class SourceInfo : public Calibrator::Info {

  public:

    //! Constructor
    SourceInfo (const SourceEstimate* source);
    
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

  protected:
    
    //! The SourceEstimate to be plotted
    Reference::To<const SourceEstimate> source;

    //! Plot all Stokes parameters in one panel
    bool together;

    //! The label printed on the y-axis
    std::string label;

  };

}

#endif
