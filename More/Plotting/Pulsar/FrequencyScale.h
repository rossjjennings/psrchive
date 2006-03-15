//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FrequencyScale.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/15 11:39:11 $
   $Author: straten $ */

#ifndef __Pulsar_FrequencyScale_h
#define __Pulsar_FrequencyScale_h

#include "Pulsar/PlotScale.h"

namespace Pulsar {

  class Archive;

  //! Plots the position angle of the linearly polarized radiation
  class FrequencyScale : public PlotScale {

  public:

    //! Default constructor
    FrequencyScale ();

    //! Return the min and max in the current units
    void get_range (const Archive*, float& min, float& max) const;

    //! Return the min and max as the phase bin index
    void get_range (const Archive*, unsigned& min, unsigned& max) const;

    //! Get the ordinate values
    void get_ordinates (const Archive*, std::vector<float>& x_axis) const;

    //! Get a description of the units
    std::string get_label ();

  protected:

  };

}

#endif
