//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_ZapExtend_h
#define __Pulsar_ZapExtend_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Archive.h"
#include "Pulsar/TimeFrequencySmooth.h"
#include "Pulsar/TimeFrequencyMask.h"

namespace Pulsar {

  //! Extend zapped areas in time and/or frequency
  /*! Extends zapped regions if more than a specified fraction
   * in either time or freq are already flagged.
   */
  class ZapExtend : public Transformation<Archive>
  {

    public:

      ZapExtend();

      void transform (Archive*);

      //! Get the text interface to the configuration attributes
      TextInterface::Parser* get_interface ();
    
      // Text interface to the ZapExtend class
      class Interface : public TextInterface::To<ZapExtend> {
      public:
        Interface (ZapExtend* = 0);
      };

      //! Set the cutoff for time-direction
      void set_time_cutoff (float t) { time_cutoff = t; }
      
      //! Get the cutoff for time-direction
      float get_time_cutoff () const { return time_cutoff; }

      //! Set the cutoff for freq-direction
      void set_freq_cutoff (float t) { freq_cutoff = t; }

      //! Get the cutoff for freq-direction
      float get_freq_cutoff () const { return freq_cutoff; }

      //! Set flag to print a one-line report
      void set_report (bool flag = true) { report = flag; }

      //! Get flag to print a one-line report
      bool get_report () const { return report; }

    protected:

      //! threshold for time-direction
      float time_cutoff;

      //! threshold for freq-direction
      float freq_cutoff;

      bool report;
  };

}

#endif
