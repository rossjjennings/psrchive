//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_TimeFrequencySmooth_h
#define __Pulsar_TimeFrequencySmooth_h

#include "Pulsar/Algorithm.h"
#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Smooth a statistic vs time freq
  /*! Base class for algorithms that smooth data vs time and frequency
   * in order to normalize out for example bandpass shape as a first
   * step in RFI removal.
   */
  class TimeFrequencySmooth : public Algorithm
  {

    public:

      TimeFrequencySmooth();

      //! Smooth the data given in raw, output to smoothed
      /*! Dimensions of input arrays should be:
       *    raw = (nsub, nchan, npol)
       *    weight = (nsub, nchan)
       *    freqs = (nsub, nchan)
       *    times = (nsub,)
       */
      virtual void smooth (std::vector<float> &smoothed, 
          std::vector<float> &raw, std::vector<float> &weight,
          std::vector<float> &freqs, std::vector<float> &times) = 0;

    protected:

      void check_dimensions (std::vector<float> &smoothed, 
          std::vector<float> &raw, std::vector<float> &weight,
          std::vector<float> &freqs, std::vector<float> &times);

      //! Index into (nsub, nchan) arrays
      unsigned idx(unsigned isubint, unsigned ichan) const { 
        return nchan*isubint + ichan;
      }

      //! Index into (nsub, nchan, npol) arrays
      unsigned idx(unsigned isubint, unsigned ichan, unsigned ipol) const { 
        return nchan*npol*isubint + npol*ichan + ipol;
      }

      // Current data dimensions
      unsigned nsub;
      unsigned nchan;
      unsigned npol;

  };

}

#endif
