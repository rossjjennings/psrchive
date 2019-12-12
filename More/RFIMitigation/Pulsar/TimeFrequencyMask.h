//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_TimeFrequencyMask_h
#define __Pulsar_TimeFrequencyMask_h

#include "Pulsar/Algorithm.h"
#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Compute mask from statistic vs time/freq
  /*! Base class for algorithms that compute a mask (set of chans/subints
   * to zap) from a statistic (eg off-pulse variance or similar) as a
   * function of time and frequency within an Archive.
   */
  class TimeFrequencyMask : public Algorithm
  {

    public:

      TimeFrequencyMask() {};

      //! Update the mask.
      /*! mask is the current weights array.
       *  stat is the relevant statistic vs time/freq
       *  model is a model for how the statistic "should" look (ie a smoothed
       *  bandpass shape or similar).
       *
       *  mask is nsubint-by-nchan, stat and model are nsubint-by-nchan-by-npol
       */
      virtual void update_mask (std::vector<float> &mask, 
          std::vector<float> &stat, std::vector<float> &model,
          unsigned nsubint, unsigned nchan, unsigned npol) = 0;

      void set_threshold (float t) { threshold = t; }
      float get_threshold () const { return threshold; }

    protected:

      float threshold;



  };

}

#endif
