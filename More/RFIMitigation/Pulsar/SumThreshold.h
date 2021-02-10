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
      unsigned update_mask (std::vector<float> &mask, 
			    std::vector<float> &stat,
			    std::vector<float> &model,
			    unsigned nsubint, unsigned nchan, unsigned npol);

      void set_nlevel ( int n ) { nlevel = n; }
      int get_nlevel () const { return nlevel; }
    
      // Text interface to the SumThreshold class
      class Interface : public TextInterface::To<SumThreshold> {
      public:
        Interface (SumThreshold* = 0);
	std::string get_interface_name () const { return "sumthresh"; }
      };

      //! Return a text interface that can be used to configure this instance
      TextInterface::Parser* get_interface ();

      // Return new instance
      SumThreshold* clone () const { return new SumThreshold(); }
    
    protected:

      //! Number of levels
      int nlevel;

  };

}

#endif
