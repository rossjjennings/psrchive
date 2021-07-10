//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/Pulsar/CalibratorSpectrum.h

#ifndef __Pulsar_CalibratorParameter_h
#define __Pulsar_CalibratorParameter_h

#include "Pulsar/FrequencyPlot.h"
#include "Pulsar/Calibrator.h"
#include "Pulsar/Index.h"

#include "EstimatePlotter.h"

namespace Pulsar {

  //! Plots a single class of model parameters from Calibrator::Info 
  class CalibratorParameter : public FrequencyPlot
  {

  public:

    //! Default constructor
    CalibratorParameter ();

    //! Return the text interface
    TextInterface::Parser* get_interface ();

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Data are managed externally (do not prepare)
    void set_managed (bool flag=true) { managed = flag; }

    //! Prepare to plot
    void prepare (const Archive*);

    //! Prepare to plot
    void prepare (const Calibrator::Info*, const Archive*);

    //! Draw the spectra
    void draw (const Archive*);

    //! Set the class index
    void set_class (unsigned i) { iclass = i; }
    //! Get the class index
    unsigned get_class () const { return iclass; }

    //! Set the outlier threshold
    void set_outlier_threshold (float t) { outlier_threshold = t; }
    //! Get the outlier threshold
    float get_outlier_threshold () const { return outlier_threshold; }

    //! Set the sub-integration Idex
    void set_subint (const Index& isub) { subint = isub; }
    //! Get the sub-integration Idex
    Index get_subint () const { return subint; }

    //! Return the Calibrator::Info from the Archive
    static Calibrator::Info* get_Info (const Archive*, Index subint,
				       float outlier_threshold);

  protected:

    EstimatePlotter plotter;

    // Text interface to the CalibratorParameter class
    class Interface;

    // Calibrator info derived from the input archive
    Reference::To<const Calibrator::Info> info;

    // The index of the class of parameters to be plotted
    unsigned iclass;

    // Flag set to true when data are managed externally (disables prepare)
    bool managed;

    // Sub-integration from which to derive a solution
    Index subint;
    
    // Threshold used to detect outliers when creating a SingleAxis calibrator
    float outlier_threshold;
  };

}

#endif
