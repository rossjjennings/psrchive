//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/JenetAnderson98Plot.h,v $
   $Revision: 1.1 $
   $Date: 2006/04/18 11:54:27 $
   $Author: straten $ */

#ifndef __JenetAnderson98_Plot_h
#define __JenetAnderson98_Plot_h

#include "dsp/JenetAnderson98.h"

namespace dsp {
  
  class TwoBitCorrection;

  //! Plots the distribution of Phi
  class JenetAnderson98::Plot  {

  public:

    //! Null constructor
    Plot ();

    //! Virtual destructor
    virtual ~Plot ();

    //! The label on the x-axis
    std::string get_xlabel () const;

    //! The label on the y-axis
    std::string get_ylabel () const;

    //! Get the colour used to plot the theoretical distribution
    int get_theory_colour () { return theory_colour; };

    //! Get a measure of the difference between the histogram and the theory
    double get_chi_squared (int idig);

    //! Plot vertical bars to indicate the cut-off thresholds
    bool show_cutoff_sigma;

    //! Plot the entire range of the distribution
    bool plot_entire_range;

    //! Interface to the data required by the plotter
    class Interface;

    //! Set the interface to the data
    void set_interface (const Interface* data) { twobit = data; }

  protected:

    //! Theoretical, optimal histogram
    std::vector<float> theory;

    //! Maxmimum value of theory
    float theory_max;

    //! Set true when the theoretical, optimal histogram is calculated
    bool theory_calculated;

    //! Colour used when plotting theoretical
    int theory_colour;

    //! Data to be plotted
    Reference::To<const Interface> twobit;

    void calculate_theory ();
    void set_theory_colour ();
    void check_colours ();
    bool special (unsigned imin, unsigned imax, float& ymax);

  };

  //! Interface to the measured distributions of Phi
  class JenetAnderson98::Plot::Interface  {

  public:

    //! Get the number of samples in each histogram
    virtual unsigned get_nsample() const = 0;

    //! Get the specified histogram
    virtual void get_histogram (std::vector<float>& histogram, unsigned idig);

    //! Return the total size of the population
    double get_histogram_total (unsigned idig);

    //! Return the fraction of low voltage samples
    double get_fraction_low();

  };


}

#endif
