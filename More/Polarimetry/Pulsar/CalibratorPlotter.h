//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CalibratorPlotter.h,v $
   $Revision: 1.13 $
   $Date: 2008/06/20 09:32:36 $
   $Author: straten $ */

#ifndef __CalibratorPlotter_H
#define __CalibratorPlotter_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  //! Plots Calibrator parameters
  class CalibratorPlotter : public Reference::Able
  {
    
  public:

    //! Verbosity flag
    static bool verbose;

    //! Default constructor
    CalibratorPlotter ();
    
    //! Destructor
    virtual ~CalibratorPlotter ();

    //! Plot the Pulsar::Calibrator
    virtual void plot (const Calibrator* calibrator);

    //! Plot the Pulsar::Calibrator
    virtual void plot (const Calibrator::Info* info, unsigned nchan,
		       double centre_frequency, double bandwidth);

    //! Number of panels (vertical)
    unsigned npanel;

    //! Fraction of total plotting space placed between panels
    float between_panels;

    //! Use colour in plots
    bool use_colour;

    //! Print titles over top of panel
    bool print_titles;

    //! Title to print over top panel
    std::string title;

  };

}

#endif
