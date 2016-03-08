//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Classes/Pulsar/PhaseResolvedHistogram.h

#ifndef __Pulsar_PhaseResolvedHistogram_h
#define __Pulsar_PhaseResolvedHistogram_h

#include "Pulsar/MoreProfiles.h"

namespace Pulsar
{
  /*! Phase-resolved histogram of some quantity */
  class PhaseResolvedHistogram : public MoreProfiles
  {
    //! minimum and maximum values spanned by the histogram
    double min, max;

  public:

    //! Construct with a name
    PhaseResolvedHistogram (const char* name = "PhaseResolvedHistogram");
    
    //! Clone operator
    PhaseResolvedHistogram* clone () const;

    //! average information from another MoreProfiles
    void average (const MoreProfiles*);

    //! Set the minimum and maximum values spanned by the histogram
    void set_range (double min, double max);

    //! get the Profile that corresponds to the specified value
    virtual Profile* at (double value);

  };

}

#endif
