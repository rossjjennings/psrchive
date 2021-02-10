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

  class ArchiveStatistic;

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
      void set_expression (const std::string&);

      //! Get the statistical expression
      std::string get_expression () const { return expression; }
    
      //! Set the statistic
      void set_statistic (ArchiveStatistic*);

      //! Get the statistic
      ArchiveStatistic* get_statistic () const;

      //! Set the smoother
      void set_smoother (TimeFrequencySmooth*);

      //! Get the smoother
      TimeFrequencySmooth* get_smoother () const;

      //! Set the masker
      void set_masker (TimeFrequencyMask*);

      //! Get the masker
      TimeFrequencyMask* get_masker () const;

      //! Set the cut-off threshold
      void set_cutoff_threshold (float t) { masker->set_threshold(t); }

      //! Get the cut-off threshold
      float get_cutoff_threshold () const { return masker->get_threshold(); }

      //! Set the maximum number of iterations
      void set_max_iterations (unsigned n) { max_iterations = n; }

      //! Get the maximum number of iterations
      unsigned get_max_iterations () const { return max_iterations; }

      //! Set the list of polns to look at
      void set_polarizations (const std::string& p) { polns = p; }

      //! Get the list of polns to look at
      std::string get_polarizations () const { return polns; }

      //! Set flag to print a one-line report
      void set_report (bool flag = true) { report = flag; }

      //! Get flag to print a one-line report
      bool get_report () const { return report; }

    protected:

      //! compute the relevant statistic
      virtual void compute_stat();

      //! determine the time/freq mask
      virtual void update_mask();

      //! The statistical expression
      std::string expression;

      //! The statistic to be derived from each profile
      Reference::To<ArchiveStatistic> statistic;

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

    //! Maximum number of times to run update_mask
    unsigned max_iterations;

    //! Print a report on stdout
    bool report;
    
  private:
    
    //! Number of subints/chans zapped during update_mask
    unsigned nmasked;

    //! Number of non-masked subints/chans in input data
    unsigned nonmasked;
  };

}

#endif
