//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Phase.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __Calibration_Phase_H
#define __Calibration_Phase_H

#include "Calibration/OptimizedComplex2.h"

namespace Calibration {

  //! A gain transformation
  class Phase : public OptimizedComplex2 {

  public:

    //! Default constructor
    Phase ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    //! Return the name of the specified parameter
    string get_param_name (unsigned index) const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedComplex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, vector<Jones<double> >* gradient);
   
  };

}

#endif
