//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotAxis.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/10 16:32:48 $
   $Author: straten $ */

#ifndef __Pulsar_PlotAxis_h
#define __Pulsar_PlotAxis_h

#include "Reference.h"

#include <utility>

namespace Pulsar {

  //! Plots the position angle of the linearly polarized radiation
  class PlotAxis : public Reference::Able {

  public:

    //! Default constructor
    PlotAxis ();

    //! Rescale min and max according to current attribute settings
    void get_range (float& min, float& max) const;

    //! Set the label to be drawn on the axis
    void set_label (const std::string& _label) { label = _label; }
    //! Get the label to be drawn on the axis
    std::string get_label () const { return label; }

    //! Set if the frame opposite the axis may have an alternate scale
    void set_alternate (bool f) { alternate = f; }
    //! Get the world-normalized minimum value on the axis
    bool get_alternate () const { return alternate; }

    //! Set the world-normalized range on the axis
    void set_range_norm (const std::pair<float,float>& f) { range_norm = f; }
    //! Get the world-normalized range on the axis
    std::pair<float,float> get_range_norm () const { return range_norm; }

    //! Set the world-normalized buffer space on either side of the axis
    void set_buf_norm (float f) { buf_norm = f; }
    //! Get the world-normalized buffer space on either side of the axis
    float get_buf_norm () const { return buf_norm; }

    //! Set the options to be passed to pgbox for this axis
    void set_pgbox_opt (const std::string& opt) { pgbox_opt = opt; }
    //! Get the label to be drawn on the axis
    std::string get_pgbox_opt () const { return pgbox_opt; }

    //! Add to the options to be passed to pgbox for this axis
    void add_pgbox_opt (char opt);

    //! Remove from the options to be passed to pgbox for this axis
    void rem_pgbox_opt (char opt);

  protected:

    std::string label;
    std::string pgbox_opt;
    bool alternate;

    std::pair<float,float> range_norm;
    float buf_norm;

  };

  
}

#endif
