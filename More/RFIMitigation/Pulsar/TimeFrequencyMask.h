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

      //! Create a new instance of TimeFrequencyMask based on name
      static TimeFrequencyMask* factory (const std::string& name);

      //! Returns a list of available TimeFrequencyMask children
      static const std::vector<TimeFrequencyMask*>& children ();

      TimeFrequencyMask();

      //! Update the mask.
      /*! mask is the current weights array.
       *  stat is the relevant statistic vs time/freq
       *  model is a model for how the statistic "should" look (ie a smoothed
       *  bandpass shape or similar).
       *
       *  mask is nsubint-by-nchan, stat and model are nsubint-by-nchan-by-npol
       *
       *  return value is number of points masked
       */
      virtual unsigned update_mask (std::vector<float> &mask, 
				    std::vector<float> &stat,
				    std::vector<float> &model,
				    unsigned nsubint,
				    unsigned nchan,
				    unsigned npol) = 0;

      virtual void set_threshold (float t) { threshold = t; }
      virtual float get_threshold () const { return threshold; }

      //! Return a text interface that can be used to configure this instance
      virtual TextInterface::Parser* get_interface () { return 0; }

      //! Derived types must also define clone method
      virtual TimeFrequencyMask* clone () const = 0;

    protected:

      float threshold;

    private:
      static void build ();
  };

  std::ostream& operator<< (std::ostream&, TimeFrequencyMask*);

  std::istream& operator>> (std::istream&, TimeFrequencyMask*&);

}

#endif
