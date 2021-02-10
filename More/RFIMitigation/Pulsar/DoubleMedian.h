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

       void set_freq_range ( float r ) { freq_range = r; }
       float get_freq_range () const { return freq_range; }
    
       void set_time_range ( float r ) { time_range = r; }
       float get_time_range () const { return time_range; }
    

      // Text floaterface to the DoubleMedian class
      class Interface : public TextInterface::To<DoubleMedian> {
      public:
        Interface (DoubleMedian* = 0);
	std::string get_interface_name () const { return "dblmed"; }
      };

      //! Return a text interface that can be used to configure this instance
      TextInterface::Parser* get_interface ();

      // Return new instance
      DoubleMedian* clone () const { return new DoubleMedian(); }
    
    protected:

      float freq_range; // MHz
      float time_range; // sec

  };

}

#endif
