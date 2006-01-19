//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/CyclicParameter.h,v $
   $Revision: 1.2 $
   $Date: 2006/01/19 15:06:04 $
   $Author: straten $ */

#ifndef __CyclicParameter_H
#define __CyclicParameter_H

#include "MEAL/OneParameter.h"
#include "Reference.h"

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

    //! Set the matching azimuth and behave like an elevation
    void set_azimuth (CyclicParameter* azimuth);

    // ///////////////////////////////////////////////////////////////////
    //
    // OneParameter implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Set the value of the parameter
    void set_param (double value);

    //! Get the value of the parameter
    double get_param () const;

  protected:

    //! Ensure that the value is within the bounds
    void check (double value);

    //! Check to see if elevation needs correction
    void check_elevation ();

  private:

    //! The lower bound on the parameter value
    double lower_bound;

    //! The upper bound on the parameter value
    double upper_bound;

    //! The period of the parameter value
    double period;

    //! The matching azimuth parameter
    Reference::To<CyclicParameter, false> azimuth;

  };

}

#endif
