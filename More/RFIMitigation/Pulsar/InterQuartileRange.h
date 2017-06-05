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
    float cutoff_threshold;
    
  public:

    //! Default construction
    InterQuartileRange ();
    
    //! Flag bad sub-integrations and frequency channels using IQR
    void transform (Archive*);

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the InterQuartileRange class
    class Interface : public TextInterface::To<InterQuartileRange> {
    public:
      Interface (InterQuartileRange* = 0);
    };

    //! Set the statistical expression
    void set_expression (const std::string& exp) { expression = exp; }

    //! Get the statistical expression
    std::string get_expression () const { return expression; }

    //! Set the cut-off threshold
    void set_cutoff_threshold (float t) { cutoff_threshold = t; }

    //! Get the cut-off threshold
    float get_cutoff_threshold () const { return cutoff_threshold; }

  };

}

#endif
