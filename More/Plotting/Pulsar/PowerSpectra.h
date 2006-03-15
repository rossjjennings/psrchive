//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PowerSpectra.h,v $
   $Revision: 1.3 $
   $Date: 2006/03/15 16:35:17 $
   $Author: straten $ */

#ifndef __Pulsar_PowerSpectra_h
#define __Pulsar_PowerSpectra_h

#include "Pulsar/FrequencyPlot.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class PowerSpectra : public FrequencyPlot {

  public:

    //! Default constructor
    PowerSpectra ();

    //! Perform any preprocessing steps
    void prepare (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the profiles
    virtual void get_spectra (const Archive*) = 0;

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the polarization to plot
    void set_pol (unsigned _ipol) { ipol = _ipol; }
    unsigned get_pol () const { return ipol; }

    //! Set the median filter window size
    void set_median_window (unsigned window) { median_window = window; }
    //! Get the median window size
    unsigned get_median_window () const { return median_window; }

    //! Set draw lines flag
    void set_draw_lines (bool flag) { draw_lines = flag; }
    //! Get draw lines flag
    bool get_draw_lines () const { return draw_lines; }

    //! Draw the spectra
    void draw (const Archive*);

    //! Draw the spectrum
    void draw (const std::vector<float>&) const;

    //! Return the text interface
    TextInterface::Class* get_interface ();

  protected:

    std::vector< std::vector<float> > spectra;
    std::vector< int > plot_sci;
    std::vector< int > plot_sls;

    unsigned isubint;
    unsigned ipol;

    unsigned median_window;
    bool draw_lines;

  private:

    std::vector<float> frequencies;

  };

}

#endif
