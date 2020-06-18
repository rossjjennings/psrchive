//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ChannelZapMedian.h

#ifndef _Pulsar_ChannelZapMedian_H
#define _Pulsar_ChannelZapMedian_H

#include "Pulsar/ChannelWeight.h"
#include "TextInterface.h"

namespace Pulsar {
  
  class Statistics;
  class ProfileStatistic;

  //! Uses a median smoothed spectrum to find birdies and zap them
  class ChannelZapMedian : public ChannelWeight
  {
    
  public:
    
    //! Default constructor
    ChannelZapMedian ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the ChannelZapMedian class
    class Interface : public TextInterface::To<ChannelZapMedian>
    {
    public:
      Interface (ChannelZapMedian* = 0);
    };

    //! Set up attributes that apply to the whole archive
    void operator () (Archive*);

    //! Set integration weights
    void weight (Integration* integration);

    //! Set the size of the window over which median will be computed
    void set_window_size (unsigned size) { window_size = size; }

    //! Get the size of the window over which median will be computed
    unsigned get_window_size () const { return window_size; }

    //! Set the threshold as multiple of the standard deviation
    void set_rms_threshold (float t);

    //! Get the threshold as multiple of the standard deviation
    float get_rms_threshold () const;

    //! Set the threshold as multiple of the median absolute deviation from the median
    void set_madm_threshold (float t);

    //! Get the threshold as multiple of the median absolute deviation from the median
    float get_madm_threshold () const;

    //! Set the threshold as multiple of the inter-quartile range
    void set_iqr_threshold (float t);

    //! Get the threshold as multiple of the inter-quartile range
    float get_iqr_threshold () const;

    //! Run the algorithm on the spectra of each bin
    void set_bybin (bool t) { bybin = t; }

    //! Run the algorithm on the spectra of each bin
    bool get_bybin () const { return bybin; }

    //! Print equivalent paz command on cout
    void set_paz_report (bool t) { paz_report = t; }

    //! Print equivalent paz command on cout
    bool get_paz_report () const { return paz_report; }

    //! Compute a single zap mask from the total
    void set_from_total (bool t) { from_total = t; }

    //! Compute a single zap mask from the total
    bool get_from_total () const { return from_total; }

    //! Set the mathematical expression
    void set_expression (const std::string& exp) { expression = exp; }

    //! Get the mathematical expression
    std::string get_expression () const { return expression; }

    //! Set the profile statistic
    void set_statistic (const std::string&);

    //! Get the profile statistic
    std::string get_statistic () const;

  protected:

    //! The size of the window over which median will be computed
    unsigned window_size;

    //! Threshold as a multiple of the standard deviation
    float rms_threshold;

    //! Threshold as a multiple of the median absolute deviation from the median
    float madm_threshold;

    //! Threshold as a multiple of the inter-quartile range
    float iqr_threshold;

    //! Median smooth the spectra of each bin
    bool bybin;

    //! Print the verbose message used by paz
    bool paz_report;

    //! Compute the zap mask from the tscrunched total
    bool from_total;

    //! The single mask to apply to every sub-integration
    std::vector<bool> single_mask;

    //! Set the expression to evaluate in each channel (as in psrstat)
    std::string expression;

    //! The Statistics estimator used to evaluate the expression
    Reference::To<Statistics> stats;
    Reference::To<TextInterface::Parser> parser;

    //! The statistic to be derived from each profile
    Reference::To<ProfileStatistic> statistic;

  };
  
}

#endif
