//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Parameters.h,v $
   $Revision: 1.4 $
   $Date: 2004/12/22 07:45:27 $
   $Author: hknight $ */

#ifndef __Parameters_H
#define __Parameters_H

#include "MEAL/ParameterBehaviour.h"

#include "EstimatePlotter.h"
#include "Estimate.h"
#include "Error.h"

#include <vector>

namespace MEAL {
  
  //! Abstract base class implements parameter storage and access
  class Parameters : public ParameterBehaviour {

  public:

    //! Default constructor
    Parameters (unsigned nparam = 0);

    //! Copy constructor
    Parameters (const Parameters& np);

    //! Equality operator
    Parameters& operator = (const Parameters& np);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return the number of parameters
    unsigned get_nparam () const
    {
      return params.size();
    }

    //! Return the value of the specified parameter
    double get_param (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_param");
      return params[index].val;
    }

    //! Set the value of the specified parameter
    void set_param (unsigned index, double value);

    //! Return the variance of the specified parameter
    double get_variance (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_variance");
      return params[index].var;
    }

    //! Set the variance of the specified parameter
    void set_variance (unsigned index, double value)
    {
      range_check (index, "MEAL::Parameters::set_variance");
      params[index].var = value;
    }

    //! Return true if parameter at index is to be fitted
    bool get_infit (unsigned index) const
    {
      range_check (index, "MEAL::Parameters::get_infit");
      return fit[index];
    }

    //! Set flag for parameter at index to be fitted
    void set_infit (unsigned index, bool flag)
    {
      range_check (index, "MEAL::Parameters::set_infit");
      fit[index] = flag;
    }

  protected:

    //! Resize params and fit arrays, setting fit=true for new parameters
    /*! This method is protected so that derived types can choose to provide
      external resize access. */
    virtual void resize (unsigned nparam);

    //! Ensure that index <= get_nparam
    void range_check (unsigned index, const char* method) const
    {
      if (index >= params.size())
	throw Error (InvalidRange, method, "index=%d >= nparam=%d", 
		     index, params.size());
    }

  private:

    //! The Estimates of the parameters
    std::vector<Estimate<double> > params;

    //! Fit flag for each Stokes parameter
    std::vector<bool> fit;

  };

}

#endif
