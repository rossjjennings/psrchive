//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotAxis.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/11 22:14:46 $
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

    //! Set if the frame opposite the axis may have an alternate scale
    void set_alternate (bool f) { alternate = f; }
    //! Get the world-normalized minimum value on the axis
    bool get_alternate () const { return alternate; }

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

  };

  
}

#endif
