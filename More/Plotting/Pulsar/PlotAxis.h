//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotAxis.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/07 23:13:23 $
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

    //! Set the label to be drawn on the axis
    void set_label (const std::string& _label) { label = _label; }
    //! Get the label to be drawn on the axis
    std::string get_label () const { return label; }

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

  protected:

    std::string label;
    std::string pgbox_opt;
    float min_norm;
    float max_norm;
    float buf_norm;

  };

}

#endif
