//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CyclicParameter.h,v $
   $Revision: 1.1 $
   $Date: 2006/01/18 23:22:14 $
   $Author: straten $ */

#ifndef __CyclicParameter_H
#define __CyclicParameter_H

#include "MEAL/OneParameter.h"

namespace MEAL {
  
  //! A periodic parameter with bounds
  /*! By default, the period is \f$2\pi\f$, the lower bound is \f$-\pi\f$
    and the upper bound is \f$\pi\f$. */

  class CyclicParameter : public OneParameter {

  public:

    //! Default constructor
    CyclicParameter (Function* context);

    //! Copy constructor
    CyclicParameter (const CyclicParameter& np);

    //! Assignment operator
    CyclicParameter& operator = (const CyclicParameter& np);

    //! Clone operator
    CyclicParameter* clone (Function* context) const;

    //! Set the lower bound on the parameter value
    void set_lower_bound (double bound);

    //! Set the upper bound on the parameter value
    void set_upper_bound (double bound);

    //! Set the period of the parameter value
    void set_period (double period);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value);

  protected:

    //! Ensure that the value is within the bounds
    void check (double value);

  private:

    //! The lower bound on the parameter value
    double lower_bound;

    //! The upper bound on the parameter value
    double upper_bound;

    //! The period of the parameter value
    double period;

  };

}

#endif
