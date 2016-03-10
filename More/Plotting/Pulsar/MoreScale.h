//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/MoreScale.h

#ifndef __Pulsar_MoreScale_h
#define __Pulsar_MoreScale_h

#include "Pulsar/PlotScale.h"

namespace Pulsar {

  class Archive;

  //! Represents an axis with radio frequency ordinate
  class MoreScale : public PlotScale {

  public:

    //! Default constructor
    MoreScale ();

    // Text interface to the MoreScale class
    class Interface : public TextInterface::To<MoreScale>
    {
    public:
      Interface (MoreScale* = 0);
    };

    TextInterface::Parser* get_interface () { return new Interface(this); }

    //! Initialize internal attributes according to Archive data
    void init (const Archive*);

    //! Return the min and max as the phase bin index
    void get_indeces (const Archive*, unsigned& min, unsigned& max) const;

    //! Get a description of the units
    std::string get_label () const;

    void set_reverse (bool flag=true) { reverse = flag; }
    bool get_reverse () const { return reverse; }

  protected:

    //! Reverse the order on the axis
    bool reverse;

  };

}

#endif
