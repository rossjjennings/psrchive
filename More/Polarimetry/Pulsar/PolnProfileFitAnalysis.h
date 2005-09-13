//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileFitAnalysis.h,v $
   $Revision: 1.1 $
   $Date: 2005/09/13 01:18:06 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileFitAnalysis_h
#define __Pulsar_PolnProfileFitAnalysis_h

#include "Pulsar/PolnProfileFit.h"
#include "Jones.h"

#include <vector>

namespace Pulsar {

  //! Analysis of the PolnProfileFit algorithm
  class PolnProfileFitAnalysis {

  public:

    //! Set the PolnProfileFit algorithm to be analysed
    void set_fit (PolnProfileFit*);

    //! Get the normalized arrival time error
    Estimate<double> get_normalized_error () const;

    //! Get the multiple correlation between phase shift and Jones parameters
    Estimate<double> get_multiple_correlation () const;

  protected:

    //! The normalized arrival time error
    Estimate<double> normalized_error;

    //! The multiple correlation between phase shift and Jones parameters
    Estimate<double> multiple_correlation;

    //! The PolnProfileFit algorithm to be analysed
    Reference::To<PolnProfileFit> fit;

    //! The partial derivative of the model gradient wrt Re[S_k]
    Jones<double> delgradient_delS (unsigned index, unsigned k) const;

    Jones<double> model_result;
    std::vector< Jones<double> > model_gradient;

    Jones<double> xform_result;
    std::vector< Jones<double> > xform_gradient;

    Jones<double> phase_result;
    std::vector< Jones<double> > phase_gradient;

  };

}

#endif
