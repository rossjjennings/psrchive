//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_SetThresholds_h
#define __Pulsar_SetThresholds_h

#include "Pulsar/TimeFrequencyMask.h"

namespace Pulsar
{
  //! Uses set cutoff values to mask bad channels and sub-integrations
  class SetThresholds : public TimeFrequencyMask
  {
    
  public:
    
    SetThresholds();
    
    unsigned update_mask (std::vector<float> &mask, 
			  std::vector<float> &stat,
			  std::vector<float> &model,
			  unsigned nsubint, unsigned nchan, unsigned npol);
    
    //! Set the cut-off threshold for the maximum value
    void set_cutoff_max (float t) { cutoff_max = t; cutoff_max_set = true; }
    
    //! Get the cut-off threshold for the maximum value
    float get_cutoff_max () const { return cutoff_max; }

    //! Set the cut-off threshold for the minimum value
    void set_cutoff_min (float t) { cutoff_min = t; cutoff_min_set = true; }

    //! Get the cut-off threshold for the minimum value
    float get_cutoff_min () const { return cutoff_min; }

    // Text interface to the SetThresholds class
    class Interface : public TextInterface::To<SetThresholds> {
    public:
      Interface (SetThresholds* = 0);
      std::string get_interface_name () const { return "set"; }
    };
    
    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();
    
    // Return new instance
    SetThresholds* clone () const { return new SetThresholds(); }
    
  protected:
    
    float cutoff_max;
    bool cutoff_max_set;
    
    float cutoff_min;
    bool cutoff_min_set;
  };

}

#endif
