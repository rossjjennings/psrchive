//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/JenetAnderson98Plot.h,v $
   $Revision: 1.3 $
   $Date: 2006/04/18 21:53:01 $
   $Author: straten $ */

#ifndef __JenetAnderson98_Plot_h
#define __JenetAnderson98_Plot_h

#include "JenetAnderson98.h"
#include "Reference.h"

//! Plots the distribution of Phi
class JenetAnderson98::Plot  {

public:

  //! Default constructor
  Plot ();
  
  //! The label on the x-axis
  std::string get_xlabel () const;
  
  //! The label on the y-axis
  std::string get_ylabel () const;
  
  //! Plot vertical bars to indicate the cut-off thresholds
  bool show_cutoff_sigma;
  
  //! Plot the entire range of the distribution
  bool plot_entire_range;
  
  //! Plot only the range between the cutoff
  bool plot_only_cut;

  //! Interface to the data required by the plotter
  class Interface;
  
  //! Set the interface to the data
  void set_interface (const Interface* data);

  //! Get the limits of the data to be plotted
  float get_xmin() const;
  float get_xmax() const;
  float get_ymin() const;
  float get_ymax() const;

  //! Plot the measured and theoretical distributions
  void plot ();

protected:
  
  //! Calculates the various quantitites of interest
  JenetAnderson98 theory;

  //! Theoretical probability distribution
  std::vector<float> theory_dist;
  
  //! Minimum value to be plotted
  float show_min;

  //! Interface to measurements to be plotted
  Reference::To<const Interface> twobit;
  
  void calculate_theory ();

private:

  void adjust_limits (const std::vector<float>& hist);
  void init_limits ();

  float ymax;
  unsigned imin;
  unsigned imax;

};

//! Interface to the measured distributions of Phi
class JenetAnderson98::Plot::Interface : public Reference::Able  {
  
public:
  
  //! Get the number of samples in each histogram
  virtual unsigned get_nsample() const = 0;
  
  //! Get the sampling threshold as a fraction of the noise power
  virtual float get_threshold () const = 0;
  
  //! Get the cut off power for impulsive interference excision
  virtual float get_cutoff_sigma () const = 0;
  
  //! Get the number of digitizers
  virtual unsigned get_ndig () const = 0;

  //! Get the specified histogram
  virtual void get_histogram (std::vector<float>& h, unsigned dig) const = 0;
  
};

#endif
