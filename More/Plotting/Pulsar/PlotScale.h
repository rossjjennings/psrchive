//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotScale.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/11 22:14:46 $
   $Author: straten $ */

#ifndef __Pulsar_PlotZoom_h
#define __Pulsar_PlotZoom_h

#include "Reference.h"

#include <utility>

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PlotZoom : public Reference::Able {

  public:

    //! Default constructor
    PlotZoom ();

    //! Rescale min and max according to current attribute settings
    void get_range (float& min, float& max) const;

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

    std::pair<float,float> range_norm;
    float buf_norm;

  };

  
}

#endif
