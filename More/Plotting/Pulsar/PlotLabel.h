//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotLabel.h,v $
   $Revision: 1.5 $
   $Date: 2006/03/15 20:06:44 $
   $Author: straten $ */

#ifndef __Pulsar_PlotLabel_h
#define __Pulsar_PlotLabel_h

#include "Reference.h"

namespace Pulsar {

  //! Stores three labels: left, centre, and right
  class PlotLabel : public Reference::Able {

  public:

    //! Default label string signifies not set by user
    static std::string unset;

    //! Default constructor
    PlotLabel ();

    //! Set the label to be drawn to the left of the frame
    void set_left (const std::string& label) { left = label; }
    //! Get the label to be drawn to the left of the frame
    std::string get_left () const { return left; }

    //! Set the label to be drawn in the centre of the frame
    void set_centre (const std::string& label) { centre = label; }
    //! Get the label to be drawn in the centre of the frame
    std::string get_centre () const { return centre; }

    //! Set the label to be drawn to the right of the frame
    void set_right (const std::string& label) { right = label; }
    //! Get the label to be drawn to the right of the frame
    std::string get_right () const { return right; }

    //! Set all labels to the specified value
    void set_all (const std::string& label) { left = right = centre = label; }

  protected:

    std::string left;
    std::string right;
    std::string centre;

  };

}

#endif
