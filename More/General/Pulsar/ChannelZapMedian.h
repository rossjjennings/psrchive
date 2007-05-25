//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelZapMedian.h,v $
   $Revision: 1.5 $
   $Date: 2007/05/25 10:59:59 $
   $Author: straten $ */

#ifndef _Pulsar_ChannelZapMedian_H
#define _Pulsar_ChannelZapMedian_H

#include "Pulsar/ChannelWeight.h"
#include "TextInterface.h"

namespace Pulsar {
  
  //! Uses a median smoothed spectrum to find birdies and zap them
  class ChannelZapMedian : public ChannelWeight {
    
  public:
    
    //! Default constructor
    ChannelZapMedian ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    // Text interface to the ChannelZapMedian class
    class Interface : public TextInterface::To<ChannelZapMedian> {
    public:
      Interface (ChannelZapMedian* = 0);
    };

    //! Set integration weights
    void weight (Integration* integration);

    //! Set the size of the window over which median will be computed
    void set_window_size (unsigned size) { window_size = size; }

    //! Get the size of the window over which median will be computed
    unsigned get_window_size () const { return window_size; }

    //! Set the cut-off threshold
    void set_cutoff_threshold (float t) { cutoff_threshold = t; }

    //! Get the cut-off threshold
    float get_cutoff_threshold () const { return cutoff_threshold; }

  protected:

    //! The size of the window over which median will be computed
    unsigned window_size;

    //! The cut-off threshold
    float cutoff_threshold;

  };
  
}

#endif
