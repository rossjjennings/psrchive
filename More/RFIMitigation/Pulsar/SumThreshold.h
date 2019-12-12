//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_SumThreshold_h
#define __Pulsar_SumThreshold_h

#include "Pulsar/Algorithm.h"
#include "Pulsar/Archive.h"
#include "Pulsar/TimeFrequencyMask.h"

namespace Pulsar {

  //! Implement the SumThreshold masking algorithm
  /*! Implement the SumThreshold masking algorithm described by
   *  Offringa et al (2010).
   */
  class SumThreshold : public TimeFrequencyMask
  {

    public:

      SumThreshold();

      //! Smooth the data given in raw, output to smoothed
      /*! Both data arrays have dims (nsub, nchan, npol), slow to fast.  weight
       * array has dims (nsub, nchan).
       */
      virtual void update_mask (std::vector<float> &mask, 
          std::vector<float> &stat, std::vector<float> &model,
          unsigned nsubint, unsigned nchan, unsigned npol);

    protected:

      //! Number of levels
      int nlevel;

  };

}

#endif
