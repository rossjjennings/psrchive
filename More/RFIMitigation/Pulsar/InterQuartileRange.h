//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_InterQuartileRange_h
#define __Pulsar_InterQuartileRange_h

#include "Pulsar/TimeFrequencyMask.h"

namespace Pulsar
{
  //! Uses the inter-quartile range to mask bad channels and sub-integrations
  /*! Implements an adjustable pair of Tukey's Fences
      https://en.wikipedia.org/wiki/Outlier#Tukey's_fences */
  class InterQuartileRange : public TimeFrequencyMask
  {
    
  public:
    
    InterQuartileRange();
    
    //! Smooth the data given in raw, output to smoothed
    /*! Both data arrays have dims (nsub, nchan, npol), slow to fast.  weight
     * array has dims (nsub, nchan).
     */
    unsigned update_mask (std::vector<float> &mask, 
			  std::vector<float> &stat,
			  std::vector<float> &model,
			  unsigned nsubint, unsigned nchan, unsigned npol);
    
    //! Set the cut-off threshold
    void set_cutoff_threshold (float t)
    { threshold = cutoff_threshold_max = cutoff_threshold_min = t; }
    
    //! Set the cut-off threshold for the maximum value
    void set_cutoff_threshold_max (float t) { cutoff_threshold_max = t; }
    
    //! Get the cut-off threshold for the maximum value
    float get_cutoff_threshold_max () const { return cutoff_threshold_max; }

    //! Set the cut-off threshold for the minimum value
    void set_cutoff_threshold_min (float t) { cutoff_threshold_min = t; }

    //! Get the cut-off threshold for the minimum value
    float get_cutoff_threshold_min () const { return cutoff_threshold_min; }

    //! Set the duty cycle used to find the 'minimum slope median'
    void set_minimum_slope_median_duty_cycle (float t)
    { minimum_slope_median_duty_cycle = t; }
    
    //! Get the duty cycle used to find the 'minimum slope median'
    float get_minimum_slope_median_duty_cycle () const
    { return minimum_slope_median_duty_cycle; }
      
    // Text interface to the InterQuartileRange class
    class Interface : public TextInterface::To<InterQuartileRange> {
    public:
      Interface (InterQuartileRange* = 0);
      std::string get_interface_name () const { return "iqr"; }
    };
    
    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();
    
    // Return new instance
    InterQuartileRange* clone () const { return new InterQuartileRange(); }
    
  protected:
    
    //! Fraction of IQR
    float cutoff_threshold_max;
    float cutoff_threshold_min;

    //! Duty cycle used to find the 'minimum slope median'
    float minimum_slope_median_duty_cycle;
  };

}

#endif
