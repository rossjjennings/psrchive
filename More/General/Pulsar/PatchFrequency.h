//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _Pulsar_PatchFrequency_H
#define _Pulsar_PatchFrequency_H

#include <vector>
#include "Pulsar/Algorithm.h"

namespace Pulsar {
  
  class Archive;
  class Integration;

  //! Fills missing frequency channels with empties
  class PatchFrequency : public Algorithm 
  {
    
  public:
    
    //! Default constructor
    PatchFrequency ();

    //! Add to A whatever is missing with respect to B, and vice versa
    void operate (Archive* A, Archive* B);

    //! Set the tolerance for matching frequencies (MHz)
    void set_frequency_tolerance (double tol) { freq_tol = tol; }

  protected:

    double freq_tol;

    // Add channels to the integration
    void add_channels (Integration *into, 
        std::vector<double> frequencies) const;

    // Add channels to the integration
    void add_channels (Archive *into, 
        std::vector<double> frequencies) const;

  };
  
}

#endif
