//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_TimeFrequencyZap_h
#define __Pulsar_TimeFrequencyZap_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Archive.h"
#include "Pulsar/TimeFrequencySmooth.h"
#include "Pulsar/TimeFrequencyMask.h"

namespace Pulsar {

  class ProfileStatistic;

  //! Find bad data using a statistic vs time and frequency
  /*! Base class for algorithms that compute a statistic as a function
   * of time and frequency within an Archive, then use this to identify
   * and zap bad channels/subints.
   */
  class TimeFrequencyZap : public Transformation<Archive>
  {

    public:

      TimeFrequencyZap();

      void transform (Archive*);

      //! Get the text interface to the configuration attributes
      TextInterface::Parser* get_interface ();
    
      // Text interface to the TimeFrequencyZap class
      class Interface : public TextInterface::To<TimeFrequencyZap> {
      public:
        Interface (TimeFrequencyZap* = 0);
      };

      //! Set the statistical expression
      void set_expression (const std::string& exp) { expression = exp; }

      //! Get the statistical expression
      std::string get_expression () const { return expression; }

      //! Set the profile statistic
      void set_statistic (const std::string&);

      //! Get the profile statistic
      std::string get_statistic () const;

      //! Set the cut-off threshold
      void set_cutoff_threshold (float t) { masker->set_threshold(t); }

      //! Get the cut-off threshold
      float get_cutoff_threshold () const { return masker->get_threshold(); }

      //! Set the list of polns to look at
      void set_polarizations (const std::string& p) { polns = p; }

      //! Get the list of polns to look at
      std::string get_polarizations () const { return polns; }

    protected:

      //! compute the relevant statistic
      virtual void compute_stat();

      //! determine the time/freq mask
      virtual void update_mask();

      //! The statistical expression
      std::string expression;

      //! The statistic to be derived from each profile
      Reference::To<ProfileStatistic> statistic;

      //! Use the archive total profile to determine on/off pulse regions
      bool regions_from_total;

      //! pscrunch first
      bool pscrunch;

      //! The list of polarizations to analyze
      std::string polns;

      //! Method to use for constructing the smoothed stat vs time/freq/pol
      Reference::To<TimeFrequencySmooth> smoother;

      //! Method to use for constructing the mask
      Reference::To<TimeFrequencyMask> masker;

      //! Copy or reference to the Archive (dedispersed)
      Reference::To<Archive> data;

      //! num channels in the current archive
      unsigned nchan;

      //! num subints in the current archive
      unsigned nsubint;

      //! num poln in the current archive
      unsigned npol;

      //! List of polarizations to analyze
      std::vector<unsigned> pol_i;

      //! Frequencies
      std::vector<float> freq;

      //! Times
      std::vector<float> time;

      //! The current statistic values
      std::vector<float> stat;

      //! The current weghts mask
      std::vector<float> mask;

      //! Index into the freqs, mask arrays vs chan and subint
      unsigned idx(unsigned isubint, unsigned ichan) const { 
        return nchan*isubint + ichan;
      }

      //! Index into the stat array vs chan and subint and pol
      unsigned idx(unsigned isubint, unsigned ichan, unsigned ipol) const { 
        return nchan*npol*isubint + npol*ichan + ipol;
      }

  };

}

#endif
