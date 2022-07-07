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
#include "Pulsar/ScrunchFactor.h"

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

      //! Set flag to compute the logarithm of the statistic
      void set_logarithmic (bool flag = true) { logarithmic = flag; }

      //! Get flag to compute the logarithm of the statistic
      bool get_logarithmic () const { return logarithmic; }

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

      //! Set the frequency scrunch factor
      void set_fscrunch (const ScrunchFactor& f) { fscrunch_factor = f; }

      //! Get the frequency scrunch factor
      const ScrunchFactor get_fscrunch () const { return fscrunch_factor; }

      //! Compute covariance matrix from bscrunched clone of data
      void set_bscrunch (const ScrunchFactor& f) { bscrunch_factor = f; }
    
      //! Get the phase bin scrunch factor
      const ScrunchFactor get_bscrunch () const { return bscrunch_factor; }

      //! Set the maximum number of iterations
      void set_max_iterations (unsigned n) { max_iterations = n; }

      //! Get the maximum number of iterations
      unsigned get_max_iterations () const { return max_iterations; }

      //! Set tasks performed on clone before computing statistic
      void set_jobs (const std::string& p) { jobs = p; }

      //! Get tasks performed on clone before computing statistic
      std::string get_jobs () const { return jobs; }

      //! Set the list of polns to look at
      void set_polarizations (const std::string& p) { polns = p; }

      //! Get the list of polns to look at
      std::string get_polarizations () const { return polns; }

      //! Set flag to recompute the statistic on each iteration
      void set_recompute (bool flag = true) { recompute = flag; }

      //! Get flag to recompute the statistic on each iteration
      bool get_recompute () const { return recompute; }

      //! Set flag to print a one-line report
      void set_report (bool flag = true) { report = flag; }

      //! Get flag to print a one-line report
      bool get_report () const { return report; }

      //! Set name of file to which data are printed
      void set_filename (const std::string& name) { filename = name; }

      //! Get name of file to which data are printed
      const std::string& get_filename () const { return filename; }

      //! Set name of file to which auxiliary data are printed
      void set_aux_filename (const std::string& name) { aux_filename = name; }

      //! Get name of file to which auxiliary data are printed
      const std::string& get_aux_filename () const { return aux_filename; }

  protected:

      //! computer the mask
      void compute_mask (Archive* data);

      //! compute the relevant statistic
      virtual void compute_stat (Archive* data);

      //! determine the time/freq mask
      virtual void update_mask();

      //! apply the current mask to the archive
      void apply_mask (Archive* archive,
		       const ScrunchFactor& fscrunch = ScrunchFactor::none,
		       unsigned chan_offset = 0);

      //! The statistical expression
      std::string expression;

      //! The statistic to be derived from each profile
      Reference::To<ArchiveStatistic> statistic;

      //! Use the logarithm of the statistic
      bool logarithmic;

      //! Use the archive total profile to determine on/off pulse regions
      bool regions_from_total;

      //! pscrunch first
      bool pscrunch;

      //! Compute mask from fscrunched clone of data (twice)
      ScrunchFactor fscrunch_factor;

      //! Compute covariance matrix from bscrunched clone of data
      ScrunchFactor bscrunch_factor;

      //! Tasks performed on clone before computing statistic
      std::string jobs;

      //! The list of polarizations to analyze
      std::string polns;

      //! Method to use for constructing the smoothed stat vs time/freq/pol
      Reference::To<TimeFrequencySmooth> smoother;

      //! Method to use for constructing the mask
      Reference::To<TimeFrequencyMask> masker;

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

      //! The current weights mask
      std::vector<float> mask;

      //! Index into the freqs, mask arrays vs chan and subint
      unsigned idx(unsigned isubint, unsigned ichan) const { 
        return nchan*isubint + ichan;
      }

      //! Index into the stat array vs chan and subint and pol
      unsigned idx(unsigned isubint, unsigned ichan, unsigned ipol) const { 
        return nchan*npol*isubint + npol*ichan + ipol;
      }

    //! One iteration of the transformation
    void iteration (Archive* archive);

    //! Maximum number of times to loop
    unsigned max_iterations;

    //! Recompute the statistic on each iteration
    bool recompute;

    //! Iterate and recompute after masking the original data
    bool recompute_original;
    
    //! Print a report on stdout
    bool report;

    //! Name of file to which statistics are printed on first iteration
    std::string filename;

    //! Name of file to which auxiliary data is printed on first iteration
    std::string aux_filename;

  private:
    
    //! Number of subints/chans zapped during update_mask
    unsigned nmasked;

    //! Number of subints/chans zapped during iteration (on original data)
    unsigned nmasked_original;

    //! Number of subints/chans zapped during iterations while computing mask
    unsigned nmasked_during_iterations;

    //! Flags for subset of sub-integrations to be computed
    /*! Optimization: recomputing can be expensive */
    std::vector<bool> compute_subint;

    //! Flags for subset of channels to be computed
    /*! Optimization: recomputing can be expensive */
    std::vector<bool> compute_chan;
    
    //! The archive that was last cloned and dedispersed
    Reference::To<Archive> last_dedispersed;
    
    //! The dedispersed clone
    Reference::To<Archive> dedispersed_clone;
  };

}

#endif
