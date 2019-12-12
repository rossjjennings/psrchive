//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_DoubleMedian_h
#define __Pulsar_DoubleMedian_h

#include "Pulsar/Algorithm.h"
#include "Pulsar/Archive.h"
#include "Pulsar/TimeFrequencySmooth.h"

namespace Pulsar {

  //! Median smooth in time then frequency
  /*! Median smooth in time direction then in frequency direction.
   * Uses physical units (MHz, seconds) rather than numbers of channels
   * to be more robust to discontinuities.
   */
  class DoubleMedian : public TimeFrequencySmooth
  {

    public:

      DoubleMedian();

      //! Smooth the data given in raw, output to smoothed
      /*! Both data arrays have dims (nsub, nchan, npol), slow to fast.  weight
       * array has dims (nsub, nchan).
       */
      virtual void smooth (std::vector<float> &smoothed, 
          std::vector<float> &raw, std::vector<float> &weight,
          std::vector<float> &freqs, std::vector<float> &times);

    protected:

      float freq_range; // MHz
      float time_range; // sec

  };

}

#endif
