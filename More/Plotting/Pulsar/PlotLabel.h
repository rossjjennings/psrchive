//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PlotLabel.h,v $
   $Revision: 1.11 $
   $Date: 2008/01/10 23:21:37 $
   $Author: straten $ */

#ifndef __Pulsar_PlotLabel_h
#define __Pulsar_PlotLabel_h

#include "TextInterface.h"

namespace Pulsar {

  class Archive;
  class ArchiveTI;

  //! Stores three labels: left, centre, and right
  class PlotLabel : public Reference::Able {

  public:

    //! Default label string signifies not set by user
    static std::string unset;

    /* UNITS: 0 => answer in normalized device coordinates
              1 => answer in inches
	      2 => answer in mm
	      3 => answer in absolute device coordinates (dots)
	      4 => answer in world coordinates
	      5 => answer as a fraction of the current viewport size
    */
    //! Coordinate frame of reference
    enum Units
    {
      //! normalized device coordinates
      Device = 0,
      //! imperial
      Inches = 1,
      //! metric
      Millimetres = 2,
      //! pixels
      Pixels = 3,
      //! world coordinates
      World = 4,
      //! fraction of the current viewport size
      Viewport = 5,
     };

    //! Default constructor
    PlotLabel ();

    //! Destructor
    virtual ~PlotLabel ();

    // Text interface to the PlotLabel class
    class Interface : public TextInterface::To<PlotLabel> {
    public:
      Interface (PlotLabel* = 0);
    };

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

    //! Set the offset between label and frame (multiple of character height)
    void set_offset (float _offset) { offset = _offset; }
    float get_offset () const { return offset; }

    //! Set the spacing between label rows (multiple of character height)
    void set_spacing (float _spacing) { spacing = _spacing; }
    float get_spacing () const { return spacing; }

    //! Set all labels to the specified value
    void set_all (const std::string& label) { left = right = centre = label; }

    //! Plot the label
    virtual void plot (const Archive*);

    //! Get the margin in the specified metric
    float get_margin (Units) const;

    //! Get the displacement of label[irow] in the specified metric
    float get_displacement (int row, Units) const;

    //! Get the displacement of label[irow] in units of the character height
    float get_displacement (int row) const;

    //! Get the number of rows in the label
    unsigned get_nrows (const std::string& label) const;

  protected:

    std::string left;
    std::string right;
    std::string centre;
    float offset;
    float spacing;

    //! Plot the label; side=0/.5/1 for left/centre/right
    void plot (const Archive*, const std::string& label, float side);

    //! Plot a single row of the label
    void row (const std::string&, unsigned irow, unsigned nrow, float side);

    //! Get the text interface to the archive class
    ArchiveTI* get_interface (const Archive*);

  private:

    Reference::To<ArchiveTI> archive_interface;
  };

}

#endif
