//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/RFIMitigation/Pulsar/InterQuartileRange.h

#ifndef __Pulsar_InterQuartileRange_h
#define __Pulsar_InterQuartileRange_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Uses the inter-quartile range to find bad channels and sub-integrations
  /*! By default, this algorithm uses the off-pulse variance as the statistic
    derived from each pulse profile and used to find outliers.  This behaviour
    can be changed by setting the expression attribute. */
  class InterQuartileRange : public Transformation<Archive>
  {

  protected:

    //! The mathematical expression to be evaluated on each Profile
    std::string expression;

    //! Fraction of IQR
    float threshold;
    
  public:

    //! Default construction
    InterQuartileRange ();
    
    //! Flag bad sub-integrations and frequency channels using IQR
    void transform (Archive*);
    
  };

}

#endif
