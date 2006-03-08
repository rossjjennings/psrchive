//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotAxis.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/08 22:33:46 $
   $Author: straten $ */

#ifndef __Pulsar_PlotAxis_h
#define __Pulsar_PlotAxis_h

#include "Reference.h"

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

    //! Set the world-normalized minimum value on the axis
    void set_min_norm (float f) { min_norm = f; }
    //! Get the world-normalized minimum value on the axis
    float get_min_norm () const { return min_norm; }

    //! Set the world-normalized maximum value on the axis
    void set_max_norm (float f) { max_norm = f; }
    //! Get the world-normalized maximum value on the axis
    float get_max_norm () const { return max_norm; }

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
    float min_norm;
    float max_norm;
    float buf_norm;

  };

}

#endif
