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
    
  private:

    unsigned lags;
    std::pair<unsigned, unsigned> covar;
  };

}

#endif
