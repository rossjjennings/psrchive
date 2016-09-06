//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/StokesCrossCovariancePlot.h

#ifndef __Pulsar_StokesCrossCovariancePlot_h
#define __Pulsar_StokesCrossCovariancePlot_h

#include "Pulsar/FluxPlot.h"
#include "Matrix.h"

namespace Pulsar {

  //! Plots a polarization pulse profile
  class StokesCrossCovariancePlot : public FluxPlot {

  public:

    //! Default constructor
    StokesCrossCovariancePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the StokesCrossCovariancePlot class
    class Interface : public TextInterface::To<StokesCrossCovariancePlot> {
    public:
      Interface (StokesCrossCovariancePlot* = 0);
    };

    //! Load the profiles
    void get_profiles (const Archive* data);

    //! Set the Stokes covariance indeces
    void set_covar (const std::pair<unsigned, unsigned>& c) { covar = c; }
    //! Get the Stokes covariance indeces
    std::pair<unsigned, unsigned> get_covar () const { return covar; }
    
    //! Set the number of lags to be plotted
    void set_lags (unsigned s) { lags = s; }
    //! Get the lags to be plotted
    unsigned get_lags () const { return lags; }

    //! Set the phase bin to be plotted
    void set_bin (int s) { bin = s; }
    //! Get the phase bin to be plotted
    int get_bin () const { return bin; }
    
    //! Search for the phase bin to be plotted
    void set_max_bin (bool s) { max_bin = s; }
    //! Search for the phase bin to be plotted
    bool get_max_bin () const { return max_bin; }
    
  private:

    unsigned lags;
    unsigned nbin;
    int bin;
    bool max_bin;
    std::pair<unsigned, unsigned> covar;
    std::vector< Matrix<4,4,double> > stokes_crossed;

    void plot_lags ();
    void plot_bin ();
      
  };

}

#endif
