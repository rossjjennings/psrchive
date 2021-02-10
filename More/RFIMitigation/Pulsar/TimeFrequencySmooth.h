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

      //! Create a new instance of TimeFrequencySmooth based on name
      static TimeFrequencySmooth* factory (const std::string& name);

      //! Returns a list of available TimeFrequencySmooth children
      static const std::vector<TimeFrequencySmooth*>& children ();

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

      //! Return a text interface that can be used to configure this instance
      virtual TextInterface::Parser* get_interface () { return 0; }

      //! Derived types must also define clone method
      virtual TimeFrequencySmooth* clone () const = 0;

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
 
    private:
      static void build ();
  };

  std::ostream& operator<< (std::ostream&, TimeFrequencySmooth*);

  std::istream& operator>> (std::istream&, TimeFrequencySmooth*&);

}

#endif
