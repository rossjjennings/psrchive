//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/StepsInfo.h,v $
   $Revision: 1.1 $
   $Date: 2007/12/06 18:34:45 $
   $Author: straten $ */

#ifndef __Pulsar_StepsInfo_H
#define __Pulsar_StepsInfo_H

#include "Pulsar/VariationInfo.h"
#include "MEAL/Steps.h"

namespace Pulsar {

  //! Communicates Steps function parameters to plotting routines
  class StepsInfo : public VariationInfo {

  public:

    //! Construct with code of which function to plot
    StepsInfo (const ReceptionCalibrator* calibrator, Which which);
    
    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
    //! Return the number of parameters in the specified class
    unsigned get_nparam (unsigned iclass) const;
    
    //! Return the estimate of the specified parameter
    Estimate<float> get_param (unsigned ichan, unsigned iclass,
			       unsigned iparam) const;

    //! Return the Steps function for the specified channel
    const MEAL::Steps* get_Steps (unsigned ichan) const;

  protected:

    // abscissa at which steps take place
    std::vector<double> steps;

    // add new steps to the steps vector
    void add_steps (const MEAL::Steps*);

    // mean value over all steps
    std::vector< MeanEstimate<double> > mean;
    std::vector< MeanRadian<double> > mean_radian;

  };

}

#endif
