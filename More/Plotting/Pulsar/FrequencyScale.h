//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FrequencyScale.h,v $
   $Revision: 1.5 $
   $Date: 2007/01/14 22:52:48 $
   $Author: straten $ */

#ifndef __Pulsar_FrequencyScale_h
#define __Pulsar_FrequencyScale_h

#include "Pulsar/PlotScale.h"

namespace Pulsar {

  class Archive;

  //! Represents an axis with radio frequency ordinate
  class FrequencyScale : public PlotScale {

  public:

    //! Default constructor
    FrequencyScale ();

    //! Initialize internal attributes according to Archive data
    void init (const Archive*);

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
