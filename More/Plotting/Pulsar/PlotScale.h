//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotScale.h,v $
   $Revision: 1.7 $
   $Date: 2006/03/16 17:07:17 $
   $Author: straten $ */

#ifndef __Pulsar_PlotScale_h
#define __Pulsar_PlotScale_h

#include "TextInterface.h"

#include <utility>

namespace Pulsar {

  class Archive;

  //! The scale on an axis
  class PlotScale : public Reference::Able {

  public:

    //! Default constructor
    PlotScale ();

    //! Text interface to the PlotScale class
    class Interface : public TextInterface::To<PlotScale> {
    public:
      Interface (PlotScale* = 0);
    };

    //! Set the minimum and maximum value in the data
    virtual void set_minmax (float min, float max);

    //! Return min and max scaled according to range_norm and buf_norm
    virtual void get_range (const Archive*, float& min, float& max) const;

    //! Return 0 < imin and imax < n, scaled according to range_norm
    virtual void get_range (unsigned n, unsigned& imin, unsigned& imax) const;

    //! Set the world-normalized range on the axis
    void set_range_norm (const std::pair<float,float>& f) { range_norm = f; }
    //! Get the world-normalized range on the axis
    std::pair<float,float> get_range_norm () const { return range_norm; }

    //! Convenience interface
    void set_range_norm (float min, float max) 
    { set_range_norm (std::pair<float,float>(min, max)); }

    //! Set the world-normalized buffer space on either side of the axis
    void set_buf_norm (float f) { buf_norm = f; }
    //! Get the world-normalized buffer space on either side of the axis
    float get_buf_norm () const { return buf_norm; }

  protected:

    float minval;
    float maxval;
    std::pair<float,float> range_norm;
    float buf_norm;

  };

  
}

#endif
