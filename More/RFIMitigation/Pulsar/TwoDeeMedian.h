//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_TwoDeeMedian_h
#define __Pulsar_TwoDeeMedian_h

#include "Pulsar/TimeFrequencySmooth.h"

namespace Pulsar {

  //! Median smooth using a 2-dimensional boxcar
  /*! Median smooth using a 2-D boxcar of specified number of
      sub-integrations and frequency channels
   */
  class TwoDeeMedian : public TimeFrequencySmooth
  {

    public:

      TwoDeeMedian();

      //! Smooth the data given in raw, output to smoothed
      /*! Both data arrays have dims (nsub, nchan, npol), slow to fast.  weight
       * array has dims (nsub, nchan).
       */
      virtual void smooth (std::vector<float> &smoothed, 
          std::vector<float> &raw, std::vector<float> &weight,
          std::vector<float> &freqs, std::vector<float> &times);

       void set_nchan ( unsigned n ) { med_nchan = n; }
       unsigned get_nchan () const { return med_nchan; }
    
       void set_nsubint ( unsigned n ) { med_nsubint = n; }
       unsigned get_nsubint () const { return med_nsubint; }

       void set_freq_range ( float r ) { freq_range = r; }
       float get_freq_range () const { return freq_range; }
    
       void set_time_range ( float r ) { time_range = r; }
       float get_time_range () const { return time_range; }
    

      // Text floaterface to the TwoDeeMedian class
      class Interface : public TextInterface::To<TwoDeeMedian> {
      public:
        Interface (TwoDeeMedian* = 0);
	std::string get_interface_name () const { return "2dmed"; }
      };

      //! Return a text interface that can be used to configure this instance
      TextInterface::Parser* get_interface ();

      // Return new instance
      TwoDeeMedian* clone () const { return new TwoDeeMedian(); }
    
    protected:

      unsigned med_nchan;
      unsigned med_nsubint;

      // TODO: also allow specification of boxcar dimensions using ...
    
      float freq_range; // MHz
      float time_range; // sec

  };

}

#endif
