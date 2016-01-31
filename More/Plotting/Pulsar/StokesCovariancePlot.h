//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/StokesCovariancePlot.h

#ifndef __Pulsar_StokesCovariancePlot_h
#define __Pulsar_StokesCovariancePlot_h

#include "Pulsar/FluxPlot.h"

namespace Pulsar {

  //! Plots a polarization pulse profile
  class StokesCovariancePlot : public FluxPlot {

  public:

    //! Default constructor
    StokesCovariancePlot ();

    //! Get the text interface to the configuration attributes
    TextInterface::Parser* get_interface ();

    // Text interface to the StokesCovariancePlot class
    class Interface : public TextInterface::To<StokesCovariancePlot> {
    public:
      Interface (StokesCovariancePlot* = 0);
    };

    //! Load the profiles
    void get_profiles (const Archive* data);

    enum What { Variances,
		EigenValues,
		RegressionCoefficients,
		ModulationIndex };

    void set_what (What w) { what = w; }
    What get_what () const { return what; }

  private:

    What what;
  };

  std::ostream& operator << (std::ostream& os, StokesCovariancePlot::What);

  std::istream& operator >> (std::istream& is, StokesCovariancePlot::What&);
}

#endif
