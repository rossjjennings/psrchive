//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelZapMedian.h,v $
   $Revision: 1.1 $
   $Date: 2005/04/20 07:37:13 $
   $Author: straten $ */

#ifndef _Pulsar_ChannelZapMedian_H
#define _Pulsar_ChannelZapMedian_H

#include "Pulsar/ChannelWeight.h"

namespace Pulsar {
  
  class ChannelZapMedian : public ChannelWeight {
    
  public:
    
    //! Default constructor
    ChannelZapMedian ();

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
